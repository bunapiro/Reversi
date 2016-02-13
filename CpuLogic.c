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
	char	aEvaluatReversiTable[HORIZON_NUM][VERTICAL_NUM][MAX_NUM];	/* �]���p�e�[�u�� */
} EVALUATE_TABLE_T;

typedef struct tagEVALUATE_TREE {
	unsigned int			Depth;					/* �[�� */
	unsigned int			NodeID;					/* �����[���ɂ����铖�Y�m�[�h��ID */
	DISC_INFO_T				DiscInfo;				/* ���Y�m�[�h�̐Ώ�� */
	unsigned int			NumNextIndex;			/* ���Y�m�[�h����h������m�[�h(�e�[�u��)�� */
	EVALUATE_TABLE_T		*pEvaluatReversiTable;	/* �]���p�e�[�u���ւ̃|�C���^ */
	int						Score;					/* ���Y�Ֆʂ̓��_ */
	struct tagEVALUATE_TREE	*pNextNode;				/* �����[��(�m�[�h)�ւ̃|�C���^(���ȎQ��) */
	struct tagEVALUATE_TREE	*pNextDepthNode;		/* ���̐[��(�m�[�h)�ւ̃|�C���^(���ȎQ��) */
} EVALUATE_TREE_T;

PRIVATE CONST int m_EvaluateWeightTable[HORIZON_NUM][VERTICAL_NUM/COEF_LINE] = 
{
			/*1*//*2*//*3*//*4*//*5*//*6*//*7*//*8*/
	/*��*/{	 30,  -12,   0,  -1,  -1,  0, -12,  30 },
	/*��*/{ -12,  -15,  -3,  -3,  -3, -3, -15, -12 },
	/*�O*/{   0,   -3,   0,  -1,  -1,  0,  -3,   0 },
	/*�l*/{  -1,   -3,  -1,  -1,  -1, -1,  -3,  -1 },
	/*��*/{  -1,   -3,  -1,  -1,  -1, -1,  -3,  -1 },
	/*�Z*/{   0,   -3,   0,  -1,  -1,  0,  -3,   0 },
	/*��*/{ -12,  -15,  -3,  -3,  -3, -3, -15, -12 },
	/*��*/{  30,  -12,   0,  -1,  -1,  0, -12,  30 },
};

/*********************************************************
API's
*********************************************************/

/******************************************************** 
 *! @brief �֐�
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
					/* �����̐΂���������_ */
					Score += m_EvaluateWeightTable[h-1][v/COEF_LINE-1];
				} else if ( CheckCellState( h, v, BLACK, aReversiTable ) ) {
					/* ����̐΂������猸�_ */
					Score -= m_EvaluateWeightTable[h-1][v/COEF_LINE-1];
				} else if ( CheckCellState( h, v, EMPTY, aReversiTable ) ) {
					;
				} else {
					printf("\n Error");
				}
			
			} else if ( Player == PLAYER_BLACK ) {
			
				if ( CheckCellState( h, v, BLACK, aReversiTable ) ) {
					/* �����̐΂���������_ */
					Score += m_EvaluateWeightTable[h-1][v/COEF_LINE-1];
				} else if ( CheckCellState( h, v, WHITE, aReversiTable ) ) {
					/* ����̐΂������猸�_ */
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
 *! @brief �֐�
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
		/* �L���Ȉʒu��T�� */
		for ( v=REALV(1); v < VERTICAL_NUM; v=v+REALV(1) ) {
			for ( h=REALH(1); h < HORIZON_NUM; h=h+REALH(1) ) {
				DiscInfo.h = h;
				DiscInfo.v = v;
				DiscInfo.CurrentPlayer = TOGGLE_PLAYER( pCurrentNode->DiscInfo.CurrentPlayer );
				Result = CheckInputPos( &DiscInfo, pCurrentNode->pEvaluatReversiTable->aEvaluatReversiTable, FALSE );
				
				/* �L���Ȉʒu����������V�����m�[�h�𐶐����� */
				if ( Result == SUCCESS ) {
					
					if ( pCurrentNode->NumNextIndex == 0 ) { /* 1�ł��L���Ȉʒu�����������玟�̐[���̃m�[�h�𐶐����� */
						
						/* Table�ւ̃|�C���^�͐������I���������ł��ė~�����̂ŃX�R�[�v�͕K�v�Œ���Ɍ��肷�� */
						EVALUATE_TABLE_T	*pNextDepthTable;
						
						/* Create Evaluate Table of Next Depth Node */
						pNextDepthTable = (EVALUATE_TABLE_T*)malloc( sizeof(EVALUATE_TABLE_T) );
						memcpy( pNextDepthTable, pCurrentNode->pEvaluatReversiTable, sizeof(EVALUATE_TABLE_T) );	/* Current�m�[�h��Table���R�s�[���� */
						ReversingDisc( DiscInfo, pNextDepthTable->aEvaluatReversiTable );							/* �R�s�[����Table�Ő΂��Ђ�����Ԃ� */
					
						/* Create Evaluate Tree Next Depth Node */
						pCurrentNode->pNextDepthNode			= (EVALUATE_TREE_T*)malloc( sizeof(EVALUATE_TREE_T) );
						pNextDepthNode							= pCurrentNode->pNextDepthNode;	/* �m�ۂ����̈�Ƀ|�C���^���q�� */
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
						
						/* �T���[�x���E���� */
						if ( pNextDepthNode != NULL ) {
							/* NextDepth���܂��T���[�x���E�ɒB���Ă��Ȃ� */
							pNextNode = pNextDepthNode->pNextNode;		/* �Ȍ�A�V�����I����������������NextNode�ɐV�KNode��ǉ����Ă��� */
						} else {
							/* NextDepth���T���[�x���E�ɒB���Ă��� */
							return;	/* �[�x���E�Ȃ̂ő��̑I������T��K�v�Ȃ� */
						}
						
					} else {	/* ���̑I����������������m�[�h�𐶐����� */
						
						/* Table�ւ̃|�C���^�͐������I���������ł��ė~�����̂ŃX�R�[�v�͕K�v�Œ���Ɍ��肷�� */
						EVALUATE_TABLE_T	*pNextTable;

						/* Create Evaluate Table of Next Node */
						pNextTable = (EVALUATE_TABLE_T*)malloc( sizeof(EVALUATE_TABLE_T) );
						memcpy( pNextTable, pCurrentNode->pEvaluatReversiTable, sizeof(EVALUATE_TABLE_T) );	/* Current�m�[�h��Table���R�s�[���� */
						ReversingDisc( DiscInfo, pNextTable->aEvaluatReversiTable );						/* �R�s�[����Table�Ő΂��Ђ�����Ԃ� */
					
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
						
						/* pNextNode�ɍ��񐶐�����Node��NextNode�A�h���X�����Ă���
						 * �V�����I����������������pNextNode�ɒǉ����Ă���		*/
						pNextNode = pNextNode->pNextNode;
					}
					
					pCurrentNode->NumNextIndex++;		/* �L���ȑI���������������玟�̐[���̃m�[�h�����C���N�������g���� */
				}
			}
		}
		
	} else {
		/* �]���؂̐[�x�����E�ɒB�����̂ŁA�m�ۂ����̈���������Tree�����I�� */
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
 *! @brief �֐�
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
	pEvaluateTreeTop->DiscInfo.CurrentPlayer= TOGGLE_PLAYER( CurrentPlayer );	/* Current�͂P��O�̑���^�[���̔Ֆ� */
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
 *! @brief �֐�
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

	/* �ċN�T���I�� */
	if ( pCurrentNode->pNextNode == NULL ) {
		pBestInfo->Score	= pCurrentNode->Score;
		pBestInfo->h		= pCurrentNode->DiscInfo.h;
		pBestInfo->v		= pCurrentNode->DiscInfo.v;
			
		/* ����Node�Ȃ̂ŗ̈��������� */
		FREE( pCurrentNode->pEvaluatReversiTable );
		FREE( pCurrentNode );
		return;
		
	} else {
  
		/* �ċN�T���J�n */
		SearchNextNode( &ReturnedBestInfo, pCurrentNode->pNextNode, CurrentPlayer );
		if ( CurrentPlayer == pCurrentNode->DiscInfo.CurrentPlayer ) {
			/* �����̃^�[���̔Ֆʂ͍ő�]���l(�����ɂƂ��čł��L��)�ƂȂ�I������I������ */
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
			/* ����̃^�[���̔Ֆʂ͍Œ�]���l(�����ɂƂ��čł��s��)�ƂȂ�I������I������ */
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
 *! @brief �֐�
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
	
	/* �]���؂̍Ő[���ɒB�����ꍇ�̏��� */
	if ( pCurrentNode->pNextDepthNode == NULL ) {
	
		if ( pCurrentNode->pNextNode == NULL ) {
			
			/* NextDepthNode��NextNode��NULL�Ȃ̂�CurrentNode�̃X�R�A����Ԃ� */
			pBestInfo->Score	= pCurrentNode->Score;
			pBestInfo->h		= pCurrentNode->DiscInfo.h;
			pBestInfo->v		= pCurrentNode->DiscInfo.v;
			
			/* ����Node�Ȃ̂ŗ̈��������� */
			FREE( pCurrentNode->pEvaluatReversiTable );
			FREE( pCurrentNode );
			return;
			
		} else {
			
			/* NextDepthNode��NULL�Ȃ̂�NextNode��T�����čőP����Z�o���� */
			SearchNextNode( &ReturnedBestInfo, pCurrentNode->pNextNode, CurrentPlayer );
			if ( CurrentPlayer == pCurrentNode->pNextNode->DiscInfo.CurrentPlayer ) {
				/* �����̃^�[���̔Ֆʂ͍ő�]���l(�����ɂƂ��čł��L��)�ƂȂ�I������I������ */
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
				/* ����̃^�[���̔Ֆʂ͍Œ�]���l(�����ɂƂ��čł��s��)�ƂȂ�I������I������ */
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
	
	/* �]���؂Ɏ��̐[�������݂���ꍇ�̏��� */
	else {
	
		/* �[���T�������s���� */
		SearchNextDepthNode( &ReturnedBestInfo, pCurrentNode->pNextDepthNode, CurrentPlayer );
		pBestInfo->Score	= ReturnedBestInfo.Score;
		pBestInfo->h		= ReturnedBestInfo.h;
		pBestInfo->v		= ReturnedBestInfo.v;
		
		/* �[���T�����I�������NextNode�̒T�������s���Ă��� */
		pSearchNode = pCurrentNode->pNextNode;
		
		while (1) {
			
			if ( pSearchNode == NULL ) {
				/* NextNode��NULL�ł����CurrentNode�̈���������NextNode�T���I�� */
				FREE( pCurrentNode->pEvaluatReversiTable );
				FREE( pCurrentNode );
				return;
			} else {
				/* NextNode�����݂���ꍇ��NextNode�ɑ΂��Đ[���T�������s���� */
				SearchNextDepthNode( &ReturnedBestInfo, pSearchNode, CurrentPlayer );
				
				if ( CurrentPlayer == pSearchNode->DiscInfo.CurrentPlayer ) {
					/* �����̃^�[���̔Ֆʂ͍ő�]���l(�����ɂƂ��čł��L��)�ƂȂ�I������I������ */
					if ( pBestInfo->Score < ReturnedBestInfo.Score ) {
						pBestInfo->Score	= ReturnedBestInfo.Score;
						pBestInfo->h		= ReturnedBestInfo.h;
						pBestInfo->v		= ReturnedBestInfo.v;
					} else {
						;
					}
				} else {
					/* ����̃^�[���̔Ֆʂ͍Œ�]���l(�����ɂƂ��čł��s��)�ƂȂ�I������I������ */
					if ( pBestInfo->Score > ReturnedBestInfo.Score ) {
						pBestInfo->Score	= ReturnedBestInfo.Score;
						pBestInfo->h		= ReturnedBestInfo.h;
						pBestInfo->v		= ReturnedBestInfo.v;
					} else {
						;
					}
				}
				
				/* SearchNode�̃|�C���^���q������ */
				pDeleteNode = pSearchNode;
				pSearchNode = pSearchNode->pNextNode;
				/* �T����������Node�͗̈��������� */
				FREE( pDeleteNode->pEvaluatReversiTable );
				FREE( pDeleteNode );
			}
		}
		
	}
	
}
/******************************************************** 
 *! @brief �֐�
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
	
	/* Tree��TopNode�͑���^�[���̔Ֆ� */
	SearchNextDepthNode( &BestInfo, pEvaluateTree, pDiscInfo->CurrentPlayer );
	
	/* EvaluateTree��NULL��������ASSERT */
	//ASSERT( SearchResult == SUCCESS );
	
	pDiscInfo->h = BestInfo.h;
	pDiscInfo->v = BestInfo.v;
}
/******************************************************** 
 *! @brief �֐�
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
 *! @brief �֐�
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
 *! @brief �R���s���[�^�^�[���R���g���[���֐�
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
