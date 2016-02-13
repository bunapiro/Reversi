#include <stdio.h>
#include <string.h>
#include "Kernel.h"
#include "Reversi.h"
#include "ReversiCommon.h"
#include "ReversiCommonDbg.h"

PRIVATE CONST char m_aHorizonPosi[END_OF_POS][MAX_NUM] =
{
	"  |", "１|", "２|", "３|", "４|", "５|", "６|", "７|", "８|",
};

PRIVATE CONST char m_aVerticalPosi[END_OF_POS][MAX_NUM] =
{
	"  |",
	"一|",
	"二|",
	"三|",
	"四|",
	"五|",
	"六|",
	"七|",
	"八|",
};

PRIVATE CONST char m_aCellState[END_OF_CELL_MARKER][MAX_NUM] =
{
	"  |",
	"---",
	"○|",
	"●|",
};

PRIVATE CONST char m_acInitReversiTable[][EIGHT][MAX_NUM] =
{
			/*1*/  /*2*/  /*3*/  /*4*/  /*5*/  /*6*/  /*7*/  /*8*/
	/*一*/{ "  |", "  |", "  |", "  |", "  |", "  |", "  |", "  |", },
	/*二*/{ "  |", "  |", "  |", "  |", "  |", "  |", "  |", "  |", },
	/*三*/{ "  |", "  |", "  |", "  |", "  |", "  |", "  |", "  |", },
	/*四*/{ "  |", "  |", "  |", "●|", "○|", "  |", "  |", "  |", },
	/*五*/{ "  |", "  |", "  |", "○|", "●|", "  |", "  |", "  |", },
	/*六*/{ "  |", "  |", "  |", "  |", "  |", "  |", "  |", "  |", },
	/*七*/{ "  |", "  |", "  |", "  |", "  |", "  |", "  |", "  |", },
	/*八*/{ "  |", "  |", "  |", "  |", "  |", "  |", "  |", "  |", },
};

/*********************************************************
API's
*********************************************************/ 

/******************************************************** 
 *! @brief 空マスチェック関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PRIVATE BOOL IsEmptyPos( REAL_POS	h,
						 REAL_POS	v,
						 char		aReversiTable[][VERTICAL_NUM][MAX_NUM] )
{
	BOOL	Result = FALSE;
	int		ResultCmp;
	
	ResultCmp = strcmp( aReversiTable[h][v], m_aCellState[EMPTY] );
	
	/* 空白のマスである */
	if ( ResultCmp == 0 ) {
		Result = TRUE;
	}
	
	return Result;
}
/******************************************************** 
 *! @brief マス状態チェック関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PUBLIC BOOL CheckCellState( REAL_POS			h,
							REAL_POS			v,
							CELL_STATE_MARKER	CellState,
							char				aReversiTable[][VERTICAL_NUM][MAX_NUM] )
{
	BOOL	Result = FALSE;
	int		ResultCmp;

	ResultCmp = strcmp( aReversiTable[h][v], m_aCellState[CellState]);
	
	/* 指定されたマスである */
	if ( ResultCmp == 0 ) {
		Result = TRUE;
	}
	
	return Result;
}
/******************************************************** 
 *! @brief 走査続行判定関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PRIVATE BOOL IsScanFinish( ERROR_CHECK	*pResult,
						   PLAYER_T		CurrentPlayer,
						   REAL_POS		h,
						   REAL_POS		v,
						   BOOL			fAdjacent,
						   char			aReversiTable[][VERTICAL_NUM][MAX_NUM] )
{
	BOOL fFinish = FALSE;

	if ( IS_OUT_OF_TABLE( h, v ) || IsEmptyPos( h, v, aReversiTable ) ) {
		/* 盤外または空マスなら終了 */
		*pResult = ERROR;
		fFinish = TRUE;
		return fFinish;
	}
			
	if ( CurrentPlayer == PLAYER_WHITE ) {
		if ( CheckCellState( h, v, WHITE, aReversiTable ) ) {
			if ( fAdjacent == FALSE ) {
				*pResult = SUCCESS;
			} else {
				*pResult = ERROR;
			}
			/* 自分の石があれば終了 */
			fFinish = TRUE;
		} else {
			/* 自分の石がなければ走査続行 */
			fFinish = FALSE;
		}
	} else if ( CurrentPlayer == PLAYER_BLACK ) {
		if ( CheckCellState( h, v, BLACK, aReversiTable ) ) {
			if ( fAdjacent == FALSE ) {
				*pResult = SUCCESS;
			} else {
				*pResult = ERROR;
			}
			/* 自分の石があれば終了 */
			fFinish = TRUE;
		} else {
			/* 自分の石がなければ走査続行 */
			fFinish = FALSE;
		}
	}

	return fFinish;
}
/******************************************************** 
 *! @brief 全方位走査関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PRIVATE void ScanDirection( DISC_INFO_T *pDiscInfo,
							char		 aReversiTable[][VERTICAL_NUM][MAX_NUM] )
{
	PLAYER_T		CurrentPlayer;
	REAL_POS		RealH, RealV;
	int				h, v;
	
	RealH			= pDiscInfo->h;
	RealV			= pDiscInfo->v;
	CurrentPlayer	= pDiscInfo->CurrentPlayer;
	
	/* 北方向 */
	for ( h=0, v=-1; ; v-- ) {
		if ( (h == 0) && (v == -1) ) {
			/* 隣接マス */
			if ( IsScanFinish( &(pDiscInfo->North), CurrentPlayer, RealH+REALH(h), RealV+REALV(v), TRUE, aReversiTable ) ) {
				break;
			}
		} else {
			/* 隣接マス以降 */
			if ( IsScanFinish( &(pDiscInfo->North), CurrentPlayer, RealH+REALH(h), RealV+REALV(v), FALSE, aReversiTable ) ) {
				break;
			}
		}
	}
	
	/* 北東方向 */
	for ( h=1, v=-1; ; h++, v-- ) {
		if ( (h == 1) && (v == -1) ) {
			/* 隣接マス */
			if ( IsScanFinish( &(pDiscInfo->NorthEast), CurrentPlayer, RealH+REALH(h), RealV+REALV(v), TRUE, aReversiTable ) ) {
				break;
			}
		} else {
			/* 隣接マス以降 */
			if ( IsScanFinish( &(pDiscInfo->NorthEast), CurrentPlayer, RealH+REALH(h), RealV+REALV(v), FALSE, aReversiTable ) ) {
				break;
			}
		}
	}
	
	/* 東方向 */
	for ( h=1, v=0; ; h++ ) {
		if ( (h == 1) && (v == 0) ) {
			/* 隣接マス */
			if ( IsScanFinish( &(pDiscInfo->East), CurrentPlayer, RealH+REALH(h), RealV+REALV(v), TRUE, aReversiTable ) ) {
				break;
			}
		} else {
			/* 隣接マス以降 */
			if ( IsScanFinish( &(pDiscInfo->East), CurrentPlayer, RealH+REALH(h), RealV+REALV(v), FALSE, aReversiTable ) ) {
				break;
			}
		}
	}
	
	/* 南東方向 */
	for ( h=1, v=1; ; h++, v++ ) {
		if ( (h == 1) && (v == 1) ) {
			/* 隣接マス */
			if ( IsScanFinish( &(pDiscInfo->SouthEast), CurrentPlayer, RealH+REALH(h), RealV+REALV(v), TRUE, aReversiTable ) ) {
				break;
			}
		} else {
			/* 隣接マス以降 */
			if ( IsScanFinish( &(pDiscInfo->SouthEast), CurrentPlayer, RealH+REALH(h), RealV+REALV(v), FALSE, aReversiTable ) ) {
				break;
			}
		}
	}
	
	/* 南方向 */
	for ( h=0, v=1; ; v++ ) {
		if ( (h == 0) && (v == 1) ) {
			/* 隣接マス */
			if ( IsScanFinish( &(pDiscInfo->South), CurrentPlayer, RealH+REALH(h), RealV+REALV(v), TRUE, aReversiTable ) ) {
				break;
			}
		} else {
			/* 隣接マス以降 */
			if ( IsScanFinish( &(pDiscInfo->South), CurrentPlayer, RealH+REALH(h), RealV+REALV(v), FALSE, aReversiTable ) ) {
				break;
			}
		}
	}
	
	/* 南西方向 */
	for ( h=-1, v=1; ; h--, v++ ) {
		if ( (h == -1) && (v == 1) ) {
			/* 隣接マス */
			if ( IsScanFinish( &(pDiscInfo->SouthWest), CurrentPlayer, RealH+REALH(h), RealV+REALV(v), TRUE, aReversiTable ) ) {
				break;
			}
		} else {
			/* 隣接マス以降 */
			if ( IsScanFinish( &(pDiscInfo->SouthWest), CurrentPlayer, RealH+REALH(h), RealV+REALV(v), FALSE, aReversiTable ) ) {
				break;
			}
		}
	}
	
	/* 西方向 */
	for ( h=-1, v=0; ; h-- ) {
		if ( (h == -1) && (v == 0) ) {
			/* 隣接マス */
			if ( IsScanFinish( &(pDiscInfo->West), CurrentPlayer, RealH+REALH(h), RealV+REALV(v), TRUE, aReversiTable ) ) {
				break;
			}
		} else {
			/* 隣接マス以降 */
			if ( IsScanFinish( &(pDiscInfo->West), CurrentPlayer, RealH+REALH(h), RealV+REALV(v), FALSE, aReversiTable ) ) {
				break;
			}
		}
	}
	
	/* 北西方向 */
	for ( h=-1, v=-1; ; h--, v-- ) {
		if ( (h == -1) && (v == -1) ) {
			/* 隣接マス */
			if ( IsScanFinish( &(pDiscInfo->NorthWest), CurrentPlayer, RealH+REALH(h), RealV+REALV(v), TRUE, aReversiTable ) ) {
				break;
			}
		} else {
			/* 隣接マス以降 */
			if ( IsScanFinish( &(pDiscInfo->NorthWest), CurrentPlayer, RealH+REALH(h), RealV+REALV(v), FALSE, aReversiTable ) ) {
				break;
			}
		}
	}

	/* 北東角 */
	pDiscInfo->EdgeOfNorthEast = ERROR;
	if (	((RealH == EIGHT) && (RealV == REALV(TWO)))
		 || ((RealH == SEVEN) && (RealV == REALV(ONE))) ) {
		/* 白のターン */
		if ( CurrentPlayer == PLAYER_WHITE ) {
			if ( CheckCellState( REALH(EIGHT), REALV(ONE), BLACK, aReversiTable ) ) {
				if (	( CheckCellState( REALH(EIGHT), REALV(TWO), WHITE, aReversiTable ) )
					 || ( CheckCellState( REALH(SEVEN), REALV(ONE), WHITE, aReversiTable ) ) ) {
					pDiscInfo->EdgeOfNorthEast = SUCCESS;
				}
			}
		/* 黒のターン */
		} else if ( CurrentPlayer == PLAYER_BLACK ) {
		     if ( CheckCellState( REALH(EIGHT), REALV(ONE), WHITE, aReversiTable ) ) {
				if (	( CheckCellState( REALH(EIGHT), REALV(TWO), BLACK, aReversiTable ) )
					 || ( CheckCellState( REALH(SEVEN), REALV(ONE), BLACK, aReversiTable ) ) ) {
					pDiscInfo->EdgeOfNorthEast = SUCCESS;
				}
			}
		}
	}
	
	/* 南東角 */
	pDiscInfo->EdgeOfSouthEast = ERROR;
	if (	((RealH == EIGHT) && (RealV == REALV(SEVEN)))
		 || ((RealH == SEVEN) && (RealV == REALV(EIGHT))) ) {
		/* 白のターン */
		if ( CurrentPlayer == PLAYER_WHITE ) {
			if ( CheckCellState( REALH(EIGHT), REALV(EIGHT), BLACK, aReversiTable ) ) {
				if (	( CheckCellState( REALH(EIGHT), REALV(SEVEN), WHITE, aReversiTable ) )
					 || ( CheckCellState( REALH(SEVEN), REALV(EIGHT), WHITE, aReversiTable ) ) ) {
					pDiscInfo->EdgeOfSouthEast = SUCCESS;
				}
			}
		/* 黒のターン */
		} else if ( CurrentPlayer == PLAYER_BLACK ) {
		     if ( CheckCellState( REALH(EIGHT), REALV(EIGHT), WHITE, aReversiTable ) ) {
				if (	( CheckCellState( REALH(EIGHT), REALV(SEVEN), BLACK, aReversiTable ) )
					 || ( CheckCellState( REALH(SEVEN), REALV(EIGHT), BLACK, aReversiTable ) ) ) {
					pDiscInfo->EdgeOfSouthEast = SUCCESS;
				}
			}
		}
	}
	
	/* 南西角 */
	pDiscInfo->EdgeOfSouthWest = ERROR;
	if (	((RealH == ONE) && (RealV == REALV(SEVEN)))
		 || ((RealH == TWO) && (RealV == REALV(EIGHT))) ) {
		/* 白のターン */
		if ( CurrentPlayer == PLAYER_WHITE ) {
			if ( CheckCellState( REALH(ONE), REALV(EIGHT), BLACK, aReversiTable ) ) {
				if (	( CheckCellState( REALH(ONE), REALV(SEVEN), WHITE, aReversiTable ) )
					 || ( CheckCellState( REALH(TWO), REALV(EIGHT), WHITE, aReversiTable ) ) ) {
					pDiscInfo->EdgeOfSouthWest = SUCCESS;
				}
			}
		/* 黒のターン */
		} else if ( CurrentPlayer == PLAYER_BLACK ) {
		     if ( CheckCellState( REALH(ONE), REALV(EIGHT), WHITE, aReversiTable ) ) {
				if (	( CheckCellState( REALH(ONE), REALV(SEVEN), BLACK, aReversiTable ) )
					 || ( CheckCellState( REALH(TWO), REALV(EIGHT), BLACK, aReversiTable ) ) ) {
					pDiscInfo->EdgeOfSouthWest = SUCCESS;
				}
			}
		}
	}
	
	
	/* 北西角 */
	pDiscInfo->EdgeOfNorthWest = ERROR;
	if (	((RealH == ONE) && (RealV == REALV(TWO)))
		 || ((RealH == TWO) && (RealV == REALV(ONE))) ) {
		/* 白のターン */
		if ( CurrentPlayer == PLAYER_WHITE ) {
			if ( CheckCellState( REALH(ONE), REALV(ONE), BLACK, aReversiTable ) ) {
				if (	( CheckCellState( REALH(ONE), REALV(TWO), WHITE, aReversiTable ) )
					 || ( CheckCellState( REALH(TWO), REALV(ONE), WHITE, aReversiTable ) ) ) {
					pDiscInfo->EdgeOfNorthWest = SUCCESS;
				}
			}
		/* 黒のターン */
		} else if ( CurrentPlayer == PLAYER_BLACK ) {
		     if ( CheckCellState( REALH(ONE), REALV(ONE), WHITE, aReversiTable ) ) {
				if (	( CheckCellState( REALH(ONE), REALV(TWO), BLACK, aReversiTable ) )
					 || ( CheckCellState( REALH(TWO), REALV(ONE), BLACK, aReversiTable ) ) ) {
					pDiscInfo->EdgeOfNorthWest = SUCCESS;
				}
			}
		}
	}
}
/******************************************************** 
 *! @brief 指し手正当性チェック関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PRIVATE BOOL IsValidPos( DISC_INFO_T *pDiscInfo,
						 char		 aReversiTable[][VERTICAL_NUM][MAX_NUM] )
{
	BOOL Result = FALSE;
	
	ScanDirection( pDiscInfo, aReversiTable );
	
	if (	(pDiscInfo->North			== SUCCESS)
		 || (pDiscInfo->NorthEast		== SUCCESS)
		 || (pDiscInfo->East			== SUCCESS)
		 || (pDiscInfo->SouthEast		== SUCCESS)
		 || (pDiscInfo->South			== SUCCESS)
		 || (pDiscInfo->SouthWest		== SUCCESS)
		 || (pDiscInfo->West			== SUCCESS)
		 || (pDiscInfo->NorthWest		== SUCCESS)
		 || (pDiscInfo->EdgeOfNorthEast == SUCCESS)
		 || (pDiscInfo->EdgeOfSouthEast == SUCCESS)
		 || (pDiscInfo->EdgeOfSouthWest == SUCCESS)
		 || (pDiscInfo->EdgeOfNorthWest == SUCCESS) ) {
		
		Result = TRUE;
	}
	
#ifdef DEBUG_VALID_POS
	printf("\n[IsValidPos]\n" HERE );
	printf("\nNorth           = %d, \nNorthEast       = %d, \nEast            = %d, \nSouthEast       = %d,	\
			\nSouth           = %d, \nSouthWest       = %d, \nWest            = %d, \nNorthWest       = %d,	\
			\nEdgeOfNorthEast = %d, \nEdgeOfSouthEast = %d, \nEdgeOfSouthWest = %d, \nEdgeOfNorthWest = %d\n"
			,pDiscInfo->North ,pDiscInfo->NorthEast ,pDiscInfo->East ,pDiscInfo->SouthEast
			,pDiscInfo->South ,pDiscInfo->SouthWest ,pDiscInfo->West ,pDiscInfo->NorthWest
			,pDiscInfo->EdgeOfNorthEast ,pDiscInfo->EdgeOfSouthEast ,pDiscInfo->EdgeOfSouthWest ,pDiscInfo->EdgeOfNorthWest );
#endif
	
	return Result;
}
/******************************************************** 
 *! @brief 石1個ひっくり返し関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PRIVATE BOOL DoReverse( PLAYER_T	CurrentPlayer,
						REAL_POS	h,
						REAL_POS	v,
						char		aReversiTable[][VERTICAL_NUM][MAX_NUM] )
{
	BOOL fFinish = FALSE;
	
	if ( IS_OUT_OF_TABLE( h, v ) ) {
		/* 盤外なら終了 */
		fFinish = TRUE;
		return fFinish;
	} else {
		/* 自分の石が来たら終了 */
		if ( CurrentPlayer == PLAYER_WHITE ) {
			if ( CheckCellState( h, v, WHITE, aReversiTable ) ) {
				fFinish = TRUE;
				return fFinish;
			}
		} else if ( CurrentPlayer == PLAYER_BLACK ) {
			if ( CheckCellState( h, v, BLACK, aReversiTable ) ) {
				fFinish = TRUE;
				return fFinish;
			}
		}
	}
	
	if ( CurrentPlayer == PLAYER_WHITE ) {
		/* 白:先攻プレイヤー */
		strcpy( aReversiTable[h][v], m_aCellState[WHITE] ); 
	} else if ( CurrentPlayer == PLAYER_BLACK ) {
		/* 黒:後攻プレイヤー */
		strcpy( aReversiTable[h][v], m_aCellState[BLACK] ); 
	}

	return fFinish;
}
/******************************************************** 
 *! @brief オセロ盤表示関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PUBLIC void PrintReversiTable( char aReversiTable[][VERTICAL_NUM][MAX_NUM] )
{
	int h, v;
	
	printf("\n");
	for ( v = 0; v < VERTICAL_NUM; v++ ){
		for ( h = 0; h < HORIZON_NUM; h++ ) {
			printf("%s" ,aReversiTable[h][v] );
		}
		printf("\n");
	}
}
/******************************************************** 
 *! @brief オセロ盤フルチェック関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PUBLIC BOOL IsReversiTableFull( char aReversiTable[][VERTICAL_NUM][MAX_NUM] )
{
	int h, v;
	
	for ( v=1; v < END_OF_POS; v++ ) {
		for ( h=1; h < END_OF_POS; h++ ) {
			if ( IsEmptyPos( REALH(h), REALV(v), aReversiTable ) ) {
				return FALSE;
			}
		}
	}
	
	return TRUE;
}
/******************************************************** 
 *! @brief 指し手チェック関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PUBLIC ERROR_CHECK CheckInputPos( DISC_INFO_T *pDiscInfo,
								  char		  aReversiTable[][VERTICAL_NUM][MAX_NUM],
								  BOOL		  fDoPrint								)
{
	ERROR_CHECK Result = SUCCESS;
	
	if ( IS_OUT_OF_TABLE( pDiscInfo->h, pDiscInfo->v ) == TRUE	) {
		if ( fDoPrint == TRUE ) {
			printf("\n******************************\n");
			printf("\n8×8の範囲で指定してください！\n");
			printf("\n******************************\n");
		}
		Result = ERROR;
	} else if ( IsEmptyPos( pDiscInfo->h, pDiscInfo->v, aReversiTable ) == FALSE ) {
		if ( fDoPrint == TRUE ) {
			printf("\n********************************\n");
			printf("\nそこには既に石が置かれています！\n");
			printf("\n********************************\n");
		}
		Result = ERROR;
	} else if ( IsValidPos( pDiscInfo, aReversiTable ) == FALSE ) {
		if ( fDoPrint == TRUE ) {
			printf("\n*******************************************\n");
			printf("\n1つ以上の石が取れる場所を選択してください！\n");
			printf("\n*******************************************\n");
		}
		Result = ERROR;
	} else {
		Result = SUCCESS;
	}
	
	return Result;
}
/******************************************************** 
 *! @brief 石ひっくり返し関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PUBLIC void ReversingDisc( DISC_INFO_T DiscInfo,
						   char		   aReversiTable[][VERTICAL_NUM][MAX_NUM] )
{
	PLAYER_T		CurrentPlayer;
	REAL_POS		RealH, RealV;
	int				h, v;
	
	RealH			= DiscInfo.h;
	RealV			= DiscInfo.v;
	CurrentPlayer	= DiscInfo.CurrentPlayer;
	
	/* 入力されたセルに石を置く */
	DoReverse( CurrentPlayer, RealH, RealV, aReversiTable );
	
	/* 北方向 */
	if ( DiscInfo.North == SUCCESS ) {
		for ( h=0, v=-1; ; v-- ) {
			if ( DoReverse( CurrentPlayer, RealH+REALH(h), RealV+REALV(v), aReversiTable ) ) {
				break;
			}
		}
	}
	
	/* 北東方向 */
	if ( DiscInfo.NorthEast == SUCCESS ) {
		for ( h=1, v=-1; ; h++, v-- ) {
			if ( DoReverse( CurrentPlayer, RealH+REALH(h), RealV+REALV(v), aReversiTable ) ) {
				break;
			}
		}
	}
	
	/* 東方向 */
	if ( DiscInfo.East == SUCCESS ) {
		for ( h=1, v=0; ; h++ ) {
			if ( DoReverse( CurrentPlayer, RealH+REALH(h), RealV+REALV(v), aReversiTable ) ) {
				break;
			}
		}
	}
	
	/* 南東方向 */
	if ( DiscInfo.SouthEast == SUCCESS ) {
		for ( h=1, v=1; ; h++, v++ ) {
			if ( DoReverse( CurrentPlayer, RealH+REALH(h), RealV+REALV(v), aReversiTable ) ) {
				break;
			}
		}
	}
	
	/* 南方向 */
	if ( DiscInfo.South == SUCCESS ) {
		for ( h=0, v=1; ; v++ ) {
			if ( DoReverse( CurrentPlayer, RealH+REALH(h), RealV+REALV(v), aReversiTable ) ) {
				break;
			}
		}
	}
	
	/* 南西方向 */
	if ( DiscInfo.SouthWest == SUCCESS ) {
		for ( h=-1, v=1; ; h--, v++ ) {
			if ( DoReverse( CurrentPlayer, RealH+REALH(h), RealV+REALV(v), aReversiTable ) ) {
				break;
			}
		}
	}
	
	/* 西方向 */
	if ( DiscInfo.West == SUCCESS ) {
		for ( h=-1, v=0; ; h-- ) {
			if ( DoReverse( CurrentPlayer, RealH+REALH(h), RealV+REALV(v), aReversiTable ) ) {
				break;
			}
		}
	}
	
	/* 北西方向 */
	if ( DiscInfo.NorthWest == SUCCESS ) {
		for ( h=-1, v=-1; ; h--, v-- ) {
			if ( DoReverse( CurrentPlayer, RealH+REALH(h), RealV+REALV(v), aReversiTable ) ) {
				break;
			}
		}
	}
	
	/* 北東角 */
	if ( DiscInfo.EdgeOfNorthEast == SUCCESS ) {
		if ( CurrentPlayer == PLAYER_WHITE ) {
			strcpy( aReversiTable[REALH(EIGHT)][REALV(ONE)], m_aCellState[WHITE] ); 
			strcpy( aReversiTable[REALH(EIGHT)][REALV(TWO)], m_aCellState[WHITE] ); 
			strcpy( aReversiTable[REALH(SEVEN)][REALV(ONE)], m_aCellState[WHITE] ); 
		} else if ( CurrentPlayer == PLAYER_BLACK ) {
			strcpy( aReversiTable[REALH(EIGHT)][REALV(ONE)], m_aCellState[BLACK] ); 
			strcpy( aReversiTable[REALH(EIGHT)][REALV(TWO)], m_aCellState[BLACK] ); 
			strcpy( aReversiTable[REALH(SEVEN)][REALV(ONE)], m_aCellState[BLACK] ); 
		}
	}
	
	/* 南東角 */
	if ( DiscInfo.EdgeOfSouthEast == SUCCESS ) {
	    if ( CurrentPlayer == PLAYER_WHITE ) {
			strcpy( aReversiTable[REALH(EIGHT)][REALV(EIGHT)], m_aCellState[WHITE] ); 
			strcpy( aReversiTable[REALH(EIGHT)][REALV(SEVEN)], m_aCellState[WHITE] ); 
			strcpy( aReversiTable[REALH(SEVEN)][REALV(EIGHT)], m_aCellState[WHITE] ); 
		} else if ( CurrentPlayer == PLAYER_BLACK ) {
			strcpy( aReversiTable[REALH(EIGHT)][REALV(EIGHT)], m_aCellState[BLACK] ); 
			strcpy( aReversiTable[REALH(EIGHT)][REALV(SEVEN)], m_aCellState[BLACK] ); 
			strcpy( aReversiTable[REALH(SEVEN)][REALV(EIGHT)], m_aCellState[BLACK] ); 
		}
	}
	
	/* 南西角 */
	if ( DiscInfo.EdgeOfSouthWest == SUCCESS ) {
		if ( CurrentPlayer == PLAYER_WHITE ) {
			strcpy( aReversiTable[REALH(ONE)][REALV(EIGHT)], m_aCellState[WHITE] ); 
			strcpy( aReversiTable[REALH(ONE)][REALV(SEVEN)], m_aCellState[WHITE] ); 
			strcpy( aReversiTable[REALH(TWO)][REALV(EIGHT)], m_aCellState[WHITE] ); 
		} else if ( CurrentPlayer == PLAYER_BLACK ) {
			strcpy( aReversiTable[REALH(ONE)][REALV(EIGHT)], m_aCellState[BLACK] ); 
			strcpy( aReversiTable[REALH(ONE)][REALV(SEVEN)], m_aCellState[BLACK] ); 
			strcpy( aReversiTable[REALH(TWO)][REALV(EIGHT)], m_aCellState[BLACK] ); 
		}
	}
	
	/* 北西角 */
	if ( DiscInfo.EdgeOfNorthWest == SUCCESS ) {
		if ( CurrentPlayer == PLAYER_WHITE ) {
			strcpy( aReversiTable[REALH(ONE)][REALV(ONE)], m_aCellState[WHITE] ); 
			strcpy( aReversiTable[REALH(ONE)][REALV(TWO)], m_aCellState[WHITE] ); 
			strcpy( aReversiTable[REALH(TWO)][REALV(ONE)], m_aCellState[WHITE] ); 
		} else if ( CurrentPlayer == PLAYER_BLACK ) {
			strcpy( aReversiTable[REALH(ONE)][REALV(ONE)], m_aCellState[BLACK] ); 
			strcpy( aReversiTable[REALH(ONE)][REALV(TWO)], m_aCellState[BLACK] ); 
			strcpy( aReversiTable[REALH(TWO)][REALV(ONE)], m_aCellState[BLACK] ); 
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
PRIVATE void LoadReversiTable( int h, 
							   int v,
							   char aReversiTable[][VERTICAL_NUM][MAX_NUM] )
{
#ifndef DEBUG_INIT_TABLE
	strcpy( aReversiTable[h][v], m_acInitReversiTable[h-1][(v/COEF_LINE)-1] );
#else
	strcpy( aReversiTable[h][v], m_acDebugInitReversiTable1[h-1][(v/COEF_LINE)-1] );
#endif
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
PUBLIC void InitReversiTable( char aReversiTable[][VERTICAL_NUM][MAX_NUM] )
{
	int	h, v;
	
	for ( v = 0; v < VERTICAL_NUM; v++ ) {
		for ( h = 0; h < HORIZON_NUM; h++ ) {
			
			if ( !(IS_MULTIPLE_OF_2(v)) ) {
				/* 2の倍数でないならラインを引く */
				strcpy( aReversiTable[h][v], m_aCellState[BAR] ); 
			} else if ( v == 0 ) {
				/* 0行目なら行番号 */
				strcpy( aReversiTable[h][v], m_aHorizonPosi[h] ); 
			} else if ( h == 0 ) {
				/* 0列目なら列番号 */
				strcpy( aReversiTable[h][v], m_aVerticalPosi[v/COEF_LINE] ); 
			} else {
				/* Load */
				LoadReversiTable( h, v, aReversiTable );
			}
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
PUBLIC void GetDiscNum( unsigned int *pNumWhite,
						unsigned int *pNumBlack,
						char		 aReversiTable[][VERTICAL_NUM][MAX_NUM] )
{
	int			 h, v;
	unsigned int NumWhite = 0, NumBlack = 0;

	for ( v=REALV(1); v < VERTICAL_NUM; v=v+REALV(1) ) {
		for ( h=REALH(1); h < HORIZON_NUM; h=h+REALH(1) ) {
			
			if ( !strcmp( aReversiTable[h][v], m_aCellState[WHITE] ) ) {
				NumWhite++;
			} else if ( !strcmp( aReversiTable[h][v], m_aCellState[BLACK] ) ) {
				NumBlack++;				
			}
		}
	}
	
	*pNumWhite = NumWhite;
	*pNumBlack = NumBlack;
}
