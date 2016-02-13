#ifndef REVERSI_COMMON_H
#define REVERSI_COMMON_H

/*******************************************************/
PUBLIC BOOL CheckCellState( REAL_POS			h,
							REAL_POS			v,
							CELL_STATE_MARKER	CellState,
							char				aReversiTable[][VERTICAL_NUM][MAX_NUM] );
/*******************************************************/
PUBLIC void PrintReversiTable( char aReversiTable[][VERTICAL_NUM][MAX_NUM] );
/*******************************************************/
PUBLIC BOOL IsReversiTableFull( char aReversiTable[][VERTICAL_NUM][MAX_NUM] );
/*******************************************************/
PUBLIC ERROR_CHECK CheckInputPos( DISC_INFO_T *pDiscInfo,
								  char		  aReversiTable[][VERTICAL_NUM][MAX_NUM],
								  BOOL		  fDoPrint								);
/*******************************************************/
PUBLIC void ReversingDisc( DISC_INFO_T DiscInfo,
						   char		   aReversiTable[][VERTICAL_NUM][MAX_NUM] );
/*******************************************************/
PUBLIC void InitReversiTable( char aReversiTable[][VERTICAL_NUM][MAX_NUM] );
/*******************************************************/
PUBLIC void GetDiscNum( unsigned int *pNumWhite,
						unsigned int *pNumBlack,
						char		 aReversiTable[][VERTICAL_NUM][MAX_NUM] );
/*******************************************************/

#endif /* REVERSI_COMMON_H */     
