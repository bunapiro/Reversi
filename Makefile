# make file

# �}�N����`
ProgramName = Reversi
ProgramObjects = ReversiCtrl.o ReversiProp.o ReversiCommon.o HumanLogic.o CpuLogic.o
CC = gcc

# �v���C�}���^�[�Q�b�g
$(ProgramName): $(ProgramObjects)
	$(CC) -o $(ProgramName) $(ProgramObjects)

# **.o�t�@�C����**.c�t�@�C���Ɉˑ����Ă���
.SUFFIXES: .c .o
.c.o:
	$(CC) -c $< 
	
# �I�u�W�F�N�g�t�@�C�����폜
.PHONY: Clean
Clean:
	$(RM) $(ProgramName) $(ProgramObjects)
