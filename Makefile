# make file

# マクロ定義
ProgramName = Reversi
ProgramObjects = ReversiCtrl.o ReversiProp.o ReversiCommon.o HumanLogic.o CpuLogic.o
CC = gcc

# プライマリターゲット
$(ProgramName): $(ProgramObjects)
	$(CC) -o $(ProgramName) $(ProgramObjects)

# **.oファイルは**.cファイルに依存している
.SUFFIXES: .c .o
.c.o:
	$(CC) -c $< 
	
# オブジェクトファイルを削除
.PHONY: Clean
Clean:
	$(RM) $(ProgramName) $(ProgramObjects)
