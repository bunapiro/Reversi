#ifndef REVERSI_PROP_H
#define REVERSI_PROP_H

/*******************************************************/
PUBLIC void IncrementTurnCount( void );
/*******************************************************/
PUBLIC void DecrementTurnCount( void );
/*******************************************************/
PUBLIC unsigned int GetTurnCount( void );
/*******************************************************/
PUBLIC void PrintTurnCount( void );
/*******************************************************/
PUBLIC void SetPlayerType( PLAYER_TYPE Player1Type, PLAYER_TYPE Player2Type );
/*******************************************************/
PUBLIC void GetPlayerType( PLAYER_TYPE *pPlayer1Type, PLAYER_TYPE *pPlayer2Type );
/*******************************************************/
PUBLIC void SetCurrentPlayer( PLAYER_T CurrentPlayer );
/*******************************************************/
PUBLIC PLAYER_T GetCurrentPlayer( void );
/*******************************************************/
PUBLIC void PrintCurrentReversiTable( void );
/*******************************************************/
PUBLIC GAME_FINISH_REASON CheckCurrentReversiTableFull( void );
/*******************************************************/
PUBLIC ERROR_CHECK CheckInputPosOnCurrent( DISC_INFO_T	*pDiscInfo,
										   BOOL			fDoPrint	 );
/*******************************************************/
PUBLIC BOOL IsExistOKNextMove( PLAYER_T Player );
/*******************************************************/
PUBLIC void UpdateCurrentReversiTable( DISC_INFO_T	DiscInfo );
/*******************************************************/
PUBLIC void CopyCurrentReversiTable( char aReversiTable[][VERTICAL_NUM][MAX_NUM],
									 SIZE_T Size									);
/*******************************************************/
PUBLIC void InitCurrentReversiTable( void );
/*******************************************************/
PUBLIC void InitializeForProperty( void );
/*******************************************************/
PUBLIC void GetCurrentDiscNum( unsigned int *pNumWhite, unsigned int *pNumBlack );
/*******************************************************/

#endif /* REVERSI_PROP_H */
