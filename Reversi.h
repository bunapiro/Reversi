#ifndef REVERSI_H
#define REVERSI_H

#define REAL_POS					unsigned int
#define COEF_LINE					2
#define HORIZON_NUM					9
#define VERTICAL_NUM				9 * COEF_LINE
#define MAX_NUM						5
#define WORK_SPACE_BOUND			sizeof(char) * HORIZON_NUM * VERTICAL_NUM * MAX_NUM 	

#define REALV(x)					x * COEF_LINE
#define REALH(x)					x
#define INIT_DISC_NUM				4
#define IS_OUT_OF_TABLE(x,y)		( (x<(REALH(1))) || (x>(REALH(8))) || (y<(REALV(1))) || (y>(REALV(8))) ) ? TRUE : FALSE

typedef enum {

	ZERO = 0,
	ONE,
	TWO,
	THREE,
	FORE,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	
	END_OF_POS,
} POS_T;

typedef enum {
	
	EMPTY	= 0,
	BAR, 
	BLACK,
	WHITE,
	
	END_OF_CELL_MARKER,
} CELL_STATE_MARKER;

typedef enum {

	PLAYER_WHITE = 0,	/* 白:先攻プレイヤー */
	PLAYER_BLACK = 1,	/* 黒:後攻プレイヤー */
} PLAYER_T;

typedef enum {

	CPU		= 0,
	HUMAN	= 1, 
} PLAYER_TYPE;

typedef enum {
	
	CONTINUOUS	= 0,
	TABLE_FULL	= 1,
	GIVE_UP		= 2,
	DEAD_LOCK	= 3, 
} GAME_FINISH_REASON;

typedef struct {
	
	PLAYER_T		CurrentPlayer;
	
	REAL_POS		h;					/* x座標 */
	REAL_POS		v;					/* y座標 */
	
	ERROR_CHECK		North;				/* 北 */
	ERROR_CHECK		NorthEast;			/* 北東 */
	ERROR_CHECK		East;				/* 東 */
	ERROR_CHECK		SouthEast;			/* 南東 */
	ERROR_CHECK		South;				/* 南 */
	ERROR_CHECK		SouthWest;			/* 南西 */
	ERROR_CHECK		West;				/* 西 */
	ERROR_CHECK		NorthWest;			/* 北西 */
	
	ERROR_CHECK		EdgeOfNorthEast;	/* 東北角 */
	ERROR_CHECK		EdgeOfSouthEast;	/* 南東角 */
	ERROR_CHECK		EdgeOfSouthWest;	/* 南西角 */
	ERROR_CHECK		EdgeOfNorthWest;	/* 北西角 */

} DISC_INFO_T;

#endif /* REVERSI_H */   
