#include <stdio.h>
#include <string.h>
#include "Kernel.h"
#include "Reversi.h"
#include "ReversiCommon.h"
#include "ReversiProp.h"
#include "HumanLogic.h"
#include "HumanLogicDbg.h"

/*********************************************************
API's
*********************************************************/

/******************************************************** 
 *! @brief 水平位置取得関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PRIVATE void GetHorizonPos( REAL_POS *pInputH )
{
	int	h;
	
	printf("\n[横方向(1～8)]\n>>");
	scanf("%d" ,&h );
	
	*pInputH = REALH(h);
}
/******************************************************** 
 *! @brief 垂直位置取得関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PRIVATE void GetVerticalPos( REAL_POS *pInputV )
{
	int v;
	
	printf("[縦方向(1～8)]\n>>");
	scanf("%d" ,&v );
	
	*pInputV = REALV(v);
}
/******************************************************** 
 *! @brief プレイヤー指し手コントロール関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PRIVATE void InputUserPos( DISC_INFO_T *pDiscInfo )
{
	REAL_POS	InputH, InputV;
	ERROR_CHECK	Result;
	
	while ( 1 ) {
		printf("\n---------------");
		if ( pDiscInfo->CurrentPlayer == PLAYER_WHITE ) {
			printf("\n [白のターンです] \n石を置く場所を選択してください:\n");
		} else if ( pDiscInfo->CurrentPlayer == PLAYER_BLACK ) {
			printf("\n [黒のターンです] \n石を置く場所を選択してください:\n");
		} else {
			printf("\n Error Current Player !!\n");
		}
		
		/* ユーザ入力値を取得 */
		GetHorizonPos( &InputH );
		GetVerticalPos( &InputV );
		
		pDiscInfo->h = InputH;
		pDiscInfo->v = InputV;
		
		/* 入力値の不正チェック */
		Result = CheckInputPosOnCurrent( pDiscInfo, TRUE );
		if (Result == SUCCESS) {
			break;
		} else {
			PrintCurrentReversiTable();
		}
	}                
	
}
/******************************************************** 
 *! @brief ヒューマンターンコントロール関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PUBLIC void HumanTurn( PLAYER_T CurrentPlayer )
{
	DISC_INFO_T DiscInfo;
	
	DiscInfo.CurrentPlayer = CurrentPlayer;
	
	InputUserPos( &DiscInfo );
	UpdateCurrentReversiTable( DiscInfo );
}
