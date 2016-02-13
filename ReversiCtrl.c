#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Kernel.h"
#include "Reversi.h"
#include "ReversiProp.h"
#include "ReversiCommon.h"
#include "ReversiCtrl.h"
#include "ReversiCtrlDbg.h"  
/*********************************************************
Define
*********************************************************/
#define IS_FINISH(x)	((x!=CONTINUOUS) ? TRUE : FALSE)

/*********************************************************
API's
*********************************************************/

/******************************************************** 
 *! @brief 手番選択関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PRIVATE void DecidePlayTurn()
{
	int			PlayerTurn;
	PLAYER_TYPE	Player1Type, Player2Type;
	
	GetPlayerType( &Player1Type, &Player2Type );
	
	if ( Player1Type == Player2Type ) {
		SetCurrentPlayer( PLAYER_WHITE );
		return;
	}
	
	while ( 1 ) {
		printf("[手番を選択してください] \n");
		printf("[0]先攻　[1]後攻 \n");
		printf(">>");
		scanf("%d" ,&PlayerTurn );
	
		if ( PlayerTurn == 0 ) {
			printf("\nあなたの先攻です\n");
			SetCurrentPlayer( PLAYER_WHITE );
	        break;
		} else if ( PlayerTurn == 1 ) {
			printf("\nあなたは後攻です\n");
			SetCurrentPlayer( PLAYER_BLACK );
	        break;
		} else {
			printf("\n入力が不正です\n");
		}
	}
}
/******************************************************** 
 *! @brief 関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PRIVATE BOOL IsPass( PLAYER_T Player )
{
	BOOL fIsPass;
	
	if ( IsExistOKNextMove( Player ) == FALSE ) {
		fIsPass = TRUE;
	} else {
		fIsPass = FALSE;		
	}
	
	return fIsPass;
}
/******************************************************** 
 *! @brief ターンプレイヤー切り替え関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PRIVATE GAME_FINISH_REASON TogglePlayer()
{
	PLAYER_T CurrentPlayer, NextPlayer;
	
	/* Dead Lock Check */
	if ( IsPass( PLAYER_WHITE ) && IsPass( PLAYER_BLACK ) ) {
		return DEAD_LOCK;
	}
	
	/* Toggle Player */
	CurrentPlayer = GetCurrentPlayer();
	
	if ( CurrentPlayer == PLAYER_WHITE ) {
		NextPlayer = PLAYER_BLACK;
	} else if ( CurrentPlayer == PLAYER_BLACK ) {
		NextPlayer = PLAYER_WHITE;
	} else {
		printf("\n Error Toggle Player !!\n");
	}
	
	/* Directive Pass Check */
	if ( IsPass( NextPlayer ) == TRUE ) {
		
		printf("\n*******************************************");
		printf("\n*                                         *");
		printf("\n*     石を置けるところがありません!       *");
		printf("\n*                                         *");
		printf("\n*******************************************\n");
		
		if ( CurrentPlayer == PLAYER_WHITE ) {
			NextPlayer = PLAYER_BLACK;
		} else if ( CurrentPlayer == PLAYER_BLACK ) {
			NextPlayer = PLAYER_WHITE;
		} else {
			printf("\n Error Toggle Player !!\n");
		}
	}
	
	SetCurrentPlayer( NextPlayer );
	
	return CONTINUOUS;
}
/******************************************************** 
 *! @brief ゲームコントロール関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PRIVATE GAME_FINISH_REASON ControlGame()
{
	PLAYER_T	CurrentPlayer;
	PLAYER_TYPE	Player1Type, Player2Type;
	
	CurrentPlayer = GetCurrentPlayer();
	GetPlayerType( &Player1Type, &Player2Type );
	
	if ( CurrentPlayer == PLAYER_WHITE ) {
		
		if ( Player1Type == CPU ) {
			
			CPUTurn( CurrentPlayer );
		} else if ( Player1Type == HUMAN ) {
			
			HumanTurn( CurrentPlayer );
		} else {
			printf("\n Error Player Type !!\n");
		}
	} else if ( CurrentPlayer == PLAYER_BLACK ) {
	
		if ( Player2Type == CPU ) {
			
			CPUTurn( CurrentPlayer );
		} else if ( Player2Type == HUMAN ) {
			
			HumanTurn( CurrentPlayer );
		} else {
			printf("\n Error Player Type !!\n");
		}
	} else {
		printf("\n Error Player Turn !!\n");
	}
	
	return CONTINUOUS;
}
/******************************************************** 
 *! @brief 関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PRIVATE void TerminateGame( GAME_FINISH_REASON Reason )
{
	unsigned int NumWhite, NumBlack;
	PLAYER_T	 CurrentPlayer;
	
	PrintCurrentReversiTable();
	printf("\n[最終結果]");
	
	switch ( Reason ) {
		
		case TABLE_FULL:
			printf("\n Game Finish Reason Table Full !!");
			GetCurrentDiscNum( &NumWhite, &NumBlack );
			printf("\n[白]:%d個" ,NumWhite );
			printf("\n[黒]:%d個" ,NumBlack );
			if ( NumWhite > NumBlack ) {
				printf("\n 白の勝ちです！！");
			} else if ( NumBlack > NumWhite ) {
				printf("\n 黒の勝ちです！！");
			} else {
				printf("\n 引き分けです");
			}
			break;
		case GIVE_UP:
			printf("\n Game Finish Reason Give UP !!");
			CurrentPlayer = GetCurrentPlayer();
			if ( CurrentPlayer == PLAYER_WHITE ) {
				printf("\n 白のプレイヤーが棄権しました");
				printf("\n 黒の勝ちです！！");
			} else if ( CurrentPlayer == PLAYER_BLACK ) {
				printf("\n 黒のプレイヤーが棄権しました");
				printf("\n 白の勝ちです！！");
			}
			break;
		case DEAD_LOCK:
			printf("\n Game Finish Reason Dead Lock !!");
			GetCurrentDiscNum( &NumWhite, &NumBlack );
			printf("\n[白]:%d個" ,NumWhite );
			printf("\n[黒]:%d個" ,NumBlack );
			if ( NumWhite > NumBlack ) {
				printf("\n 白の勝ちです！！");
			} else if ( NumBlack > NumWhite ) {
				printf("\n 黒の勝ちです！！");
			} else {
				printf("\n 引き分けです");
			}
			break;
		default:
			printf("\n Game Finish Reason Illegal !!");
			break;
	}
}
/******************************************************** 
 *! @brief ゲームシーケンスコントロール関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PRIVATE void ControlReversiSequence()
{
	GAME_FINISH_REASON Result = CONTINUOUS;
	
#ifdef DEBUG_INIT_TABLE
	/* 盤が埋まっていたら終了 */
	Result = CheckCurrentReversiTableFull();
	if ( IS_FINISH( Result ) ) {
		TerminateGame( Result );
	}
#endif
	
	while (1) {
		
		/* (0).準備 */
		IncrementTurnCount();
		
		/* (1).盤面を表示 */
		PrintTurnCount();
		PrintCurrentReversiTable();
		
		/* (2).各プレイヤーのターン */
		Result = ControlGame();
		if ( IS_FINISH( Result ) ) {
			break;
		}
		
		/* (3).盤が埋まっていたら終了 */
		Result = CheckCurrentReversiTableFull();
		if ( IS_FINISH( Result ) ) {
			break;
		}
		
		/* (4).手番を入れ替える */
		Result = TogglePlayer();
		if ( IS_FINISH( Result ) ) {
			break;
		}
		
	}
	
	TerminateGame( Result );
}
/******************************************************** 
 *! @brief オセロプレイヤー選択関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PRIVATE void SelectReversiPlayer()
{
	int			PlayerType;
	
	while ( 1 ) {
		printf("\n[対戦形式を選択してください] \n");
		printf("[0]人 vs 人　[1]人 vs CPU　[2]CPU vs CPU \n");
		printf(">>");
		scanf("%d" ,&PlayerType );
		
		if ( PlayerType == 0 ) {
			SetPlayerType( HUMAN, HUMAN );
	        break;
		} else if ( PlayerType == 1 ) {
			SetPlayerType( HUMAN, CPU );
	        break;
		} else if ( PlayerType == 2 ) {
			SetPlayerType( CPU, CPU );
	        break;
		} else {
			printf("\n入力が不正です\n\n");
		}
	}
}
/******************************************************** 
 *! @brief 関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PRIVATE void Initialize( void )
{
	InitCurrentReversiTable();
	InitializeForProperty();
}
/******************************************************** 
 *! @brief main関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PUBLIC int main( void )
{	
	Initialize();
	SelectReversiPlayer();
	DecidePlayTurn();
	ControlReversiSequence();	
	
	return EXIT_SUCCESS;
}
