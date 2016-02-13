#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Kernel.h"
#include "Reversi.h"
#include "ReversiCommon.h"
#include "ReversiProp.h"
#include "CpuLogic.h" 
#include "CpuLogicDbg.h" 
/*********************************************************
Define
*********************************************************/
#define DEPTH_MAX			3
#define	TOGGLE_PLAYER(x)	((x==PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE) 

typedef struct {
	int			Score;
	REAL_POS	h;
	REAL_POS	v;
} BEST_TABLE_INFO_T;

typedef struct {
	char	aEvaluatReversiTable[HORIZON_NUM][VERTICAL_NUM][MAX_NUM];	/* 評価用テーブル */
} EVALUATE_TABLE_T;

typedef struct tagEVALUATE_TREE {
	unsigned int			Depth;					/* 深さ */
	unsigned int			NodeID;					/* 同じ深さにおける当該ノードのID */
	DISC_INFO_T				DiscInfo;				/* 当該ノードの石情報 */
	unsigned int			NumNextIndex;			/* 当該ノードから派生するノード(テーブル)数 */
	EVALUATE_TABLE_T		*pEvaluatReversiTable;	/* 評価用テーブルへのポインタ */
	int						Score;					/* 当該盤面の得点 */
	struct tagEVALUATE_TREE	*pNextNode;				/* 同じ深さ(ノード)へのポインタ(自己参照) */
	struct tagEVALUATE_TREE	*pNextDepthNode;		/* 次の深さ(ノード)へのポインタ(自己参照) */
} EVALUATE_TREE_T;

PRIVATE CONST int m_EvaluateWeightTable[HORIZON_NUM][VERTICAL_NUM/COEF_LINE] = 
{
			/*1*//*2*//*3*//*4*//*5*//*6*//*7*//*8*/
	/*一*/{	 30,  -12,   0,  -1,  -1,  0, -12,  30 },
	/*二*/{ -12,  -15,  -3,  -3,  -3, -3, -15, -12 },
	/*三*/{   0,   -3,   0,  -1,  -1,  0,  -3,   0 },
	/*四*/{  -1,   -3,  -1,  -1,  -1, -1,  -3,  -1 },
	/*五*/{  -1,   -3,  -1,  -1,  -1, -1,  -3,  -1 },
	/*六*/{   0,   -3,   0,  -1,  -1,  0,  -3,   0 },
	/*七*/{ -12,  -15,  -3,  -3,  -3, -3, -15, -12 },
	/*八*/{  30,  -12,   0,  -1,  -1,  0, -12,  30 },
};

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
PRIVATE int CalcScore(	PLAYER_T Player,
						char	 aReversiTable[][VERTICAL_NUM][MAX_NUM] )
{
	int h, v;
	int Score = 0;
	
	for ( v=REALV(1); v < VERTICAL_NUM; v=v+REALV(1) ) {
		for ( h=REALH(1); h < HORIZON_NUM; h=h+REALH(1) ) {
		
			if ( Player == PLAYER_WHITE ) {
				
				if ( CheckCellState( h, v, WHITE, aReversiTable ) ) {
					/* 自分の石だったら加点 */
					Score += m_EvaluateWeightTable[h-1][v/COEF_LINE-1];
				} else if ( CheckCellState( h, v, BLACK, aReversiTable ) ) {
					/* 相手の石だったら減点 */
					Score -= m_EvaluateWeightTable[h-1][v/COEF_LINE-1];
				} else if ( CheckCellState( h, v, EMPTY, aReversiTable ) ) {
					;
				} else {
					printf("\n Error");
				}
			
			} else if ( Player == PLAYER_BLACK ) {
			
				if ( CheckCellState( h, v, BLACK, aReversiTable ) ) {
					/* 自分の石だったら加点 */
					Score += m_EvaluateWeightTable[h-1][v/COEF_LINE-1];
				} else if ( CheckCellState( h, v, WHITE, aReversiTable ) ) {
					/* 相手の石だったら減点 */
					Score -= m_EvaluateWeightTable[h-1][v/COEF_LINE-1];
				} else if ( CheckCellState( h, v, EMPTY, aReversiTable ) ) {
					;
				} else {
					printf("\n Error");
				}
				
			}
		
		}
	}
	
	return Score;
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
PRIVATE void MakeTree( EVALUATE_TREE_T *pCurrentNode )
{
	int					h, v;
	DISC_INFO_T			DiscInfo;
	ERROR_CHECK			Result;
	EVALUATE_TREE_T		*pNextDepthNode;
	EVALUATE_TREE_T		*pNextNode;
	
	if ( pCurrentNode->Depth <= DEPTH_MAX ) {
#ifdef DEBUG_MAKE_NODE
		printf("\nDepth = %d, NodeID = %d, CurrentPlayer = %d, Pos = [%d, %d], Score = %d"
				,pCurrentNode->Depth ,pCurrentNode->NodeID ,pCurrentNode->DiscInfo.CurrentPlayer ,pCurrentNode->DiscInfo.h
				,pCurrentNode->DiscInfo.v/COEF_LINE ,pCurrentNode->Score );
		PrintReversiTable( pCurrentNode->pEvaluatReversiTable->aEvaluatReversiTable );
#endif
		/* 有効な位置を探す */
		for ( v=REALV(1); v < VERTICAL_NUM; v=v+REALV(1) ) {
			for ( h=REALH(1); h < HORIZON_NUM; h=h+REALH(1) ) {
				DiscInfo.h = h;
				DiscInfo.v = v;
				DiscInfo.CurrentPlayer = TOGGLE_PLAYER( pCurrentNode->DiscInfo.CurrentPlayer );
				Result = CheckInputPos( &DiscInfo, pCurrentNode->pEvaluatReversiTable->aEvaluatReversiTable, FALSE );
				
				/* 有効な位置を見つけたら新しいノードを生成する */
				if ( Result == SUCCESS ) {
					
					if ( pCurrentNode->NumNextIndex == 0 ) { /* 1つでも有効な位置が見つかったら次の深さのノードを生成する */
						
						/* Tableへのポインタは生成し終わったら消滅して欲しいのでスコープは必要最低限に限定する */
						EVALUATE_TABLE_T	*pNextDepthTable;
						
						/* Create Evaluate Table of Next Depth Node */
						pNextDepthTable = (EVALUATE_TABLE_T*)malloc( sizeof(EVALUATE_TABLE_T) );
						memcpy( pNextDepthTable, pCurrentNode->pEvaluatReversiTable, sizeof(EVALUATE_TABLE_T) );	/* CurrentノードのTableをコピーする */
						ReversingDisc( DiscInfo, pNextDepthTable->aEvaluatReversiTable );							/* コピーしたTableで石をひっくり返す */
					
						/* Create Evaluate Tree Next Depth Node */
						pCurrentNode->pNextDepthNode			= (EVALUATE_TREE_T*)malloc( sizeof(EVALUATE_TREE_T) );
						pNextDepthNode							= pCurrentNode->pNextDepthNode;	/* 確保した領域にポインタを繋ぐ */
						pNextDepthNode->Depth					= pCurrentNode->Depth + 1;
						pNextDepthNode->NodeID					= pCurrentNode->NumNextIndex;
						pNextDepthNode->DiscInfo.h				= DiscInfo.h;
						pNextDepthNode->DiscInfo.v				= DiscInfo.v;
						pNextDepthNode->DiscInfo.CurrentPlayer	= DiscInfo.CurrentPlayer;
						pNextDepthNode->pEvaluatReversiTable	= pNextDepthTable;
						pNextDepthNode->Score					= CalcScore( pNextDepthNode->DiscInfo.CurrentPlayer,
																			 pNextDepthNode->pEvaluatReversiTable->aEvaluatReversiTable );
						pNextDepthNode->pNextNode				= NULL;
						pNextDepthNode->pNextDepthNode			= NULL;
						
						/* Create Evaluate Tree Next Next Depth Node */
						MakeTree( pNextDepthNode );
						
						/* 探索深度限界判定 */
						if ( pNextDepthNode != NULL ) {
							/* NextDepthがまだ探索深度限界に達していない */
							pNextNode = pNextDepthNode->pNextNode;		/* 以後、新しい選択肢が見つかったらNextNodeに新規Nodeを追加していく */
						} else {
							/* NextDepthが探索深度限界に達していた */
							return;	/* 深度限界なので他の選択肢を探る必要なし */
						}
						
					} else {	/* 次の選択肢が見つかったらノードを生成する */
						
						/* Tableへのポインタは生成し終わったら消滅して欲しいのでスコープは必要最低限に限定する */
						EVALUATE_TABLE_T	*pNextTable;

						/* Create Evaluate Table of Next Node */
						pNextTable = (EVALUATE_TABLE_T*)malloc( sizeof(EVALUATE_TABLE_T) );
						memcpy( pNextTable, pCurrentNode->pEvaluatReversiTable, sizeof(EVALUATE_TABLE_T) );	/* CurrentノードのTableをコピーする */
						ReversingDisc( DiscInfo, pNextTable->aEvaluatReversiTable );						/* コピーしたTableで石をひっくり返す */
					
						/* Create Evaluate Tree Next Node */
						pNextNode								= (EVALUATE_TREE_T*)malloc( sizeof(EVALUATE_TREE_T) );
						pNextNode->Depth						= pCurrentNode->pNextDepthNode->Depth;
						pNextNode->NodeID						= pCurrentNode->NumNextIndex;
						pNextNode->DiscInfo.h					= DiscInfo.h;
						pNextNode->DiscInfo.v					= DiscInfo.v;
						pNextNode->DiscInfo.CurrentPlayer		= DiscInfo.CurrentPlayer;
						pNextNode->pEvaluatReversiTable			= pNextTable;
						pNextNode->Score						= CalcScore( pNextNode->DiscInfo.CurrentPlayer,
																			 pNextNode->pEvaluatReversiTable->aEvaluatReversiTable );
						pNextNode->pNextNode					= NULL;
						pNextNode->pNextDepthNode				= NULL;
						
						/* Create Evaluate Tree Next Depth Next Node */
						MakeTree( pNextNode );
						
						/* pNextNodeに今回生成したNodeのNextNodeアドレスを入れておく
						 * 新しい選択肢が見つかったらpNextNodeに追加していく		*/
						pNextNode = pNextNode->pNextNode;
					}
					
					pCurrentNode->NumNextIndex++;		/* 有効な選択肢が見つかったら次の深さのノード数をインクリメントする */
				}
			}
		}
		
	} else {
		/* 評価木の深度が限界に達したので、確保した領域を解放してTree生成終了 */
		FREE( pCurrentNode->pEvaluatReversiTable );
		FREE( pCurrentNode );
		return;
	}
#ifdef DEBUG_MAKE_NODE
	printf("\n [Depth = %d] NumNextIndex = %d" ,pCurrentNode->Depth ,pCurrentNode->NumNextIndex );
#endif
	
	return;
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
PRIVATE EVALUATE_TREE_T* CreateEvaluateTree( PLAYER_T CurrentPlayer )
{
	EVALUATE_TREE_T		*pEvaluateTreeTop;	
	EVALUATE_TABLE_T	*pReversiTableTop;
	SIZE_T				AllocateAreaSize;
	unsigned int		Depth;
	
	/* Create Evaluate Table of Top Node */
	pReversiTableTop = (EVALUATE_TABLE_T*)malloc( sizeof(EVALUATE_TABLE_T) );
	AllocateAreaSize = sizeof( pReversiTableTop->aEvaluatReversiTable );
	CopyCurrentReversiTable( pReversiTableTop->aEvaluatReversiTable, AllocateAreaSize );
	
	/* Create Evaluate Tree Top Node */
	pEvaluateTreeTop = (EVALUATE_TREE_T*)malloc( sizeof(EVALUATE_TREE_T) );
	pEvaluateTreeTop->Depth					= 0;
	pEvaluateTreeTop->NodeID				= 0;
	pEvaluateTreeTop->DiscInfo.h			= 0;
	pEvaluateTreeTop->DiscInfo.v			= 0;
	pEvaluateTreeTop->DiscInfo.CurrentPlayer= TOGGLE_PLAYER( CurrentPlayer );	/* Currentは１手前の相手ターンの盤面 */
	pEvaluateTreeTop->NumNextIndex			= 0;
	pEvaluateTreeTop->pEvaluatReversiTable	= pReversiTableTop;
	pEvaluateTreeTop->Score					= CalcScore( pEvaluateTreeTop->DiscInfo.CurrentPlayer,
														 pEvaluateTreeTop->pEvaluatReversiTable->aEvaluatReversiTable );
	pEvaluateTreeTop->pNextNode				= NULL;
	pEvaluateTreeTop->pNextDepthNode		= NULL;
	
	/* Create Tree */
	MakeTree( pEvaluateTreeTop );
	
	return pEvaluateTreeTop;
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
PRIVATE void SearchNextNode( BEST_TABLE_INFO_T	*pBestInfo,
							 EVALUATE_TREE_T	*pCurrentNode,
							 PLAYER_T			CurrentPlayer )
{
	EVALUATE_TREE_T		pSearchNode;
	BEST_TABLE_INFO_T	ReturnedBestInfo;

	/* 再起探索終了 */
	if ( pCurrentNode->pNextNode == NULL ) {
		pBestInfo->Score	= pCurrentNode->Score;
		pBestInfo->h		= pCurrentNode->DiscInfo.h;
		pBestInfo->v		= pCurrentNode->DiscInfo.v;
			
		/* 末尾Nodeなので領域を解放する */
		FREE( pCurrentNode->pEvaluatReversiTable );
		FREE( pCurrentNode );
		return;
		
	} else {
  
		/* 再起探索開始 */
		SearchNextNode( &ReturnedBestInfo, pCurrentNode->pNextNode, CurrentPlayer );
		if ( CurrentPlayer == pCurrentNode->DiscInfo.CurrentPlayer ) {
			/* 自分のターンの盤面は最大評価値(自分にとって最も有利)となる選択肢を選択する */
			if ( pCurrentNode->Score < ReturnedBestInfo.Score ) {
				pBestInfo->Score	= ReturnedBestInfo.Score;
				pBestInfo->h		= ReturnedBestInfo.h;
				pBestInfo->v		= ReturnedBestInfo.v;
			} else {
				pBestInfo->Score	= pCurrentNode->Score;
				pBestInfo->h		= pCurrentNode->DiscInfo.h;
				pBestInfo->v		= pCurrentNode->DiscInfo.v;
			}
		} else {
			/* 相手のターンの盤面は最低評価値(自分にとって最も不利)となる選択肢を選択する */
			if ( pCurrentNode->Score > ReturnedBestInfo.Score ) {
				pBestInfo->Score	= ReturnedBestInfo.Score;
				pBestInfo->h		= ReturnedBestInfo.h;
				pBestInfo->v		= ReturnedBestInfo.v;
			} else {
				pBestInfo->Score	= pCurrentNode->Score;
				pBestInfo->h		= pCurrentNode->DiscInfo.h;
				pBestInfo->v		= pCurrentNode->DiscInfo.v;
			}
		}
		
		FREE( pCurrentNode->pEvaluatReversiTable );
		FREE( pCurrentNode );
		
		return;
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
PRIVATE void SearchNextDepthNode( BEST_TABLE_INFO_T	*pBestInfo,
										 EVALUATE_TREE_T	*pCurrentNode,
										 PLAYER_T			CurrentPlayer )
{
	EVALUATE_TREE_T		*pSearchNode;
	EVALUATE_TREE_T		*pDeleteNode;
	BEST_TABLE_INFO_T	ReturnedBestInfo;
	ERROR_CHECK			Result;
	
	/* 評価木の最深部に達した場合の処理 */
	if ( pCurrentNode->pNextDepthNode == NULL ) {
	
		if ( pCurrentNode->pNextNode == NULL ) {
			
			/* NextDepthNodeもNextNodeもNULLなのでCurrentNodeのスコア情報を返す */
			pBestInfo->Score	= pCurrentNode->Score;
			pBestInfo->h		= pCurrentNode->DiscInfo.h;
			pBestInfo->v		= pCurrentNode->DiscInfo.v;
			
			/* 末尾Nodeなので領域を解放する */
			FREE( pCurrentNode->pEvaluatReversiTable );
			FREE( pCurrentNode );
			return;
			
		} else {
			
			/* NextDepthNodeがNULLなのでNextNodeを探索して最善手を算出する */
			SearchNextNode( &ReturnedBestInfo, pCurrentNode->pNextNode, CurrentPlayer );
			if ( CurrentPlayer == pCurrentNode->pNextNode->DiscInfo.CurrentPlayer ) {
				/* 自分のターンの盤面は最大評価値(自分にとって最も有利)となる選択肢を選択する */
				if ( pCurrentNode->Score < ReturnedBestInfo.Score ) {
					pBestInfo->Score	= ReturnedBestInfo.Score;
					pBestInfo->h		= ReturnedBestInfo.h;
					pBestInfo->v		= ReturnedBestInfo.v;
				} else {
					pBestInfo->Score	= pCurrentNode->Score;
					pBestInfo->h		= pCurrentNode->DiscInfo.h;
					pBestInfo->v		= pCurrentNode->DiscInfo.v;
				}
			} else {
				/* 相手のターンの盤面は最低評価値(自分にとって最も不利)となる選択肢を選択する */
				if ( pCurrentNode->Score > ReturnedBestInfo.Score ) {
					pBestInfo->Score	= ReturnedBestInfo.Score;
					pBestInfo->h		= ReturnedBestInfo.h;
					pBestInfo->v		= ReturnedBestInfo.v;
				} else {
					pBestInfo->Score	= pCurrentNode->Score;
					pBestInfo->h		= pCurrentNode->DiscInfo.h;
					pBestInfo->v		= pCurrentNode->DiscInfo.v;
				}
			}
			
			FREE( pCurrentNode->pEvaluatReversiTable );
			FREE( pCurrentNode );
			return;
		}
	}
	
	/* 評価木に次の深さが存在する場合の処理 */
	else {
	
		/* 深さ探索を実行する */
		SearchNextDepthNode( &ReturnedBestInfo, pCurrentNode->pNextDepthNode, CurrentPlayer );
		pBestInfo->Score	= ReturnedBestInfo.Score;
		pBestInfo->h		= ReturnedBestInfo.h;
		pBestInfo->v		= ReturnedBestInfo.v;
		
		/* 深さ探索が終わったらNextNodeの探索を実行していく */
		pSearchNode = pCurrentNode->pNextNode;
		
		while (1) {
			
			if ( pSearchNode == NULL ) {
				/* NextNodeがNULLであればCurrentNode領域を解放してNextNode探索終了 */
				FREE( pCurrentNode->pEvaluatReversiTable );
				FREE( pCurrentNode );
				return;
			} else {
				/* NextNodeが存在する場合はNextNodeに対して深さ探索を実行する */
				SearchNextDepthNode( &ReturnedBestInfo, pSearchNode, CurrentPlayer );
				
				if ( CurrentPlayer == pSearchNode->DiscInfo.CurrentPlayer ) {
					/* 自分のターンの盤面は最大評価値(自分にとって最も有利)となる選択肢を選択する */
					if ( pBestInfo->Score < ReturnedBestInfo.Score ) {
						pBestInfo->Score	= ReturnedBestInfo.Score;
						pBestInfo->h		= ReturnedBestInfo.h;
						pBestInfo->v		= ReturnedBestInfo.v;
					} else {
						;
					}
				} else {
					/* 相手のターンの盤面は最低評価値(自分にとって最も不利)となる選択肢を選択する */
					if ( pBestInfo->Score > ReturnedBestInfo.Score ) {
						pBestInfo->Score	= ReturnedBestInfo.Score;
						pBestInfo->h		= ReturnedBestInfo.h;
						pBestInfo->v		= ReturnedBestInfo.v;
					} else {
						;
					}
				}
				
				/* SearchNodeのポインタを繋ぎ直す */
				pDeleteNode = pSearchNode;
				pSearchNode = pSearchNode->pNextNode;
				/* 探索完了したNodeは領域を解放する */
				FREE( pDeleteNode->pEvaluatReversiTable );
				FREE( pDeleteNode );
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
PRIVATE void MiniMaxMethod( EVALUATE_TREE_T	*pEvaluateTree,
							DISC_INFO_T		*pDiscInfo		)
{
	BEST_TABLE_INFO_T BestInfo;
	ERROR_CHECK		  SearchResult;
	
	/* TreeのTopNodeは相手ターンの盤面 */
	SearchNextDepthNode( &BestInfo, pEvaluateTree, pDiscInfo->CurrentPlayer );
	
	/* EvaluateTreeがNULLだったらASSERT */
	//ASSERT( SearchResult == SUCCESS );
	
	pDiscInfo->h = BestInfo.h;
	pDiscInfo->v = BestInfo.v;
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
PRIVATE void SearchEvaluateTree( EVALUATE_TREE_T *pEvaluateTree, DISC_INFO_T *pDiscInfo )
{
	MiniMaxMethod( pEvaluateTree, pDiscInfo );
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
PRIVATE void DecideDiscPos( DISC_INFO_T *pDiscInfo )
{
	EVALUATE_TREE_T *pEvaluateTree;
	
	pEvaluateTree = CreateEvaluateTree( pDiscInfo->CurrentPlayer );
	SearchEvaluateTree( pEvaluateTree, pDiscInfo );
}
/******************************************************** 
 *! @brief コンピュータターンコントロール関数
	-
@param[out]     -
@param[in]      -
@par            -
@return         -
@note      
*********************************************************/
PUBLIC void CPUTurn( PLAYER_T CurrentPlayer )
{
	DISC_INFO_T DiscInfo;
	
	DiscInfo.CurrentPlayer = CurrentPlayer;
	
	//DecideDiscPos( &DiscInfo );
	//UpdateCurrentReversiTable( DiscInfo );
	
}
