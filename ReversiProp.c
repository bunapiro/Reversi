#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include "Kernel.h"
#include "Reversi.h"
#include "ReversiCommon.h"
#include "ReversiProp.h"
#include "ReversiPropDbg.h"
/*********************************************************
Define
*********************************************************/
PRIVATE char			m_aReversiTable[HORIZON_NUM][VERTICAL_NUM][MAX_NUM];
PRIVATE PLAYER_T		m_CurrentPlayer;
PRIVATE PLAYER_TYPE		m_Player1Type, m_Player2Type;
PRIVATE unsigned int	m_TurnCount;

/*********************************************************
API's
*********************************************************/

/******************************************************** 
 *! @brief 関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PRIVATE void InitTurnCount( void )
{
	unsigned int NumWhite, NumBlack, NumTotal;
	
	GetDiscNum( &NumWhite, &NumBlack, m_aReversiTable );	
	
	NumTotal	= NumWhite + NumBlack;
	m_TurnCount = NumTotal - INIT_DISC_NUM;
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
PUBLIC void IncrementTurnCount( void )
{
	m_TurnCount++;
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
PUBLIC void DecrementTurnCount( void )
{
	m_TurnCount--;
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
PUBLIC unsigned int GetTurnCount( void )
{
	return m_TurnCount;
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
PUBLIC void PrintTurnCount( void )
{
	printf("\n[現在:%dターン目]" ,m_TurnCount );
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
PUBLIC void SetPlayerType( PLAYER_TYPE Player1Type, PLAYER_TYPE Player2Type )
{
	m_Player1Type = Player1Type;
	m_Player2Type = Player2Type;
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
PUBLIC void GetPlayerType( PLAYER_TYPE *pPlayer1Type, PLAYER_TYPE *pPlayer2Type )
{
	*pPlayer1Type = m_Player1Type;
	*pPlayer2Type = m_Player2Type;
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
PUBLIC void SetCurrentPlayer( PLAYER_T CurrentPlayer )
{
	m_CurrentPlayer = CurrentPlayer;
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
PUBLIC PLAYER_T GetCurrentPlayer( void )
{
	return m_CurrentPlayer;
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
PUBLIC void PrintCurrentReversiTable( void )
{
	PrintReversiTable( m_aReversiTable );
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
PUBLIC GAME_FINISH_REASON CheckCurrentReversiTableFull( void )
{
	BOOL				fIsFull;
	GAME_FINISH_REASON	Result = CONTINUOUS;
	
	fIsFull = IsReversiTableFull( m_aReversiTable );
	
	if ( fIsFull == TRUE ) {
		Result = TABLE_FULL;
	}
	
	return Result;
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
PUBLIC ERROR_CHECK CheckInputPosOnCurrent( DISC_INFO_T	*pDiscInfo,
										   BOOL			fDoPrint	 )
{
	ERROR_CHECK		Result;
	
	Result = CheckInputPos( pDiscInfo, m_aReversiTable, fDoPrint );

	return Result;
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
PUBLIC BOOL IsExistOKNextMove( PLAYER_T Player )
{
	int			h, v;
	DISC_INFO_T DiscInfo;
	ERROR_CHECK Result;
	
	for ( v=REALV(1); v < VERTICAL_NUM; v=v+REALV(1) ) {
		for ( h=REALH(1); h < HORIZON_NUM; h=h+REALH(1) ) {
			DiscInfo.h = h;
			DiscInfo.v = v;
			DiscInfo.CurrentPlayer = Player;
			Result = CheckInputPosOnCurrent( &DiscInfo, FALSE );
			
			if ( Result == SUCCESS ) {
				return TRUE;
			}
		}
	}
	
	return FALSE;
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
PUBLIC void UpdateCurrentReversiTable( DISC_INFO_T	DiscInfo )
{
	ReversingDisc( DiscInfo, m_aReversiTable );
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
PUBLIC void GetCurrentDiscNum( unsigned int *pNumWhite, unsigned int *pNumBlack )
{
	GetDiscNum( pNumWhite, pNumBlack, m_aReversiTable );
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
PUBLIC void CopyCurrentReversiTable( char aReversiTable[][VERTICAL_NUM][MAX_NUM],
									 SIZE_T Size									)
{
	if ( Size < WORK_SPACE_BOUND ) {
		printf("\nSpecified Memory Size: %d" ,Size );
		printf("\nNeed Memory Size: %d" ,WORK_SPACE_BOUND );
		err( EXIT_FAILURE, "\nInsufficiency Specified Memory Size !!\n" );
	}
	
	memcpy( aReversiTable, m_aReversiTable, WORK_SPACE_BOUND );
}
/******************************************************** 
 *! @brief オセロ盤初期化関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PUBLIC void InitCurrentReversiTable( void )
{
	InitReversiTable( m_aReversiTable );
}
/******************************************************** 
 *! @brief 初期化関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PUBLIC void InitializeForProperty( void )
{
	m_Player1Type	= HUMAN;
	m_Player2Type	= HUMAN;
	m_CurrentPlayer	= PLAYER_WHITE;
	InitTurnCount();
}
