#!/usr/bin/perl

##########################################################################
# Athena�p�f�[�^�o�b�N�A�b�v�c�[��
#
# �@Athena�̊e��f�[�^�t�@�C��*.txt���o�b�N�A�b�v����c�[��
#
#-------------------------------------------------------------------------
# �ݒ���@
# �@���s���鎞�̃J�����g�t�H���_����̃f�[�^�ւ̃p�X�A�t�@�C���̃��X�g��
# �@�������ݒ肵�܂��B�o�b�N�A�b�v��̃t�H���_�͎����쐬����Ȃ��̂ŁA
# �@�����ō쐬���Ă����K�v������܂��B
# �@�t�H���_�̍Ō�́u/�v�͏ȗ��ł��܂���B
#
# �@�t�H���_�͈����ł��w��ł��܂��B�၄./backup ../save/ ./backup_data/
# �@�t�H���_�̍Ō�́u/�v�͏ȗ��ł��܂���B
#
# �@���s����ƃo�b�N�A�b�v��̃t�H���_�ցA�t�@�C�����Ɍ��݂̓��t�Ǝ�����
# �@���ăt�@�C�����R�s�[���܂��B
#
#   * tool�t�H���_����backup_data�t�H���_���쐬���A
# �@  athena.sh�̒��Ɂu./tool/backup ./save/ ./tool/backup_data/�v
#     �Ƃ����s��ǉ�����ƁAathena���N�����邽�тɃo�b�N�A�b�v�����܂�
#
#   ��������Ƃ��͈����Ɂu-r ���t�Ǝ����v���w�肵�܂��B
# �@�܂����̌��Ƀt�H���_���w�肷�邱�Ƃ��o���܂�
# �@��P�� ./backup -r 200309191607
# �@��Q�� ./backup -r 200309191607 ../save ./backup_data/
# �@���̗�ł�2003/09/19��16:07���Ƀo�b�N�A�b�v�����f�[�^�𕜌����Ă��܂�
#
# �@��������Ƃ��AAthena�f�B���N�g���ɂ���f�[�^�� *.bak �ɖ��O��ύX����
# �@�c���Ă���̂ŁA����Ȃ��ꍇ�� rm *.bak �Ȃǂŏ����Ă��������B
#
##########################################################################

$sdir="../save/";		#�o�b�N�A�b�v��(Athena�̃f�B���N�g��/save/)
$tdir="./backup_data/";		#�o�b�N�A�b�v��

@files=(			#�t�@�C���̃��X�g
	"account","athena","storage","party","guild","castle","pet"
);


#-------------------------------�ݒ肱���܂�-----------------------------











if($ARGV[0]=~/^\-r$/i || $ARGV[0]=~/\-\-(recover|restore)/i){
	#��������

	$file=$ARGV[1];
	$sdir=$ARGV[2]||$sdir;
	$tdir=$ARGV[3]||$tdir;
	&restorecopy($_) foreach @files;
	exit(0);
}

#�o�b�N�A�b�v����
$sdir=$ARGV[0]||$sdir;
$tdir=$ARGV[1]||$tdir;

unless( -d $tdir ){
	print "$0: \"$tdir\" : No such directory\n";
	exit(1);
}

(undef,$min,$hour,$day,$month,$year)=localtime;

$file=sprintf("%04d%02d%02d%02d%02d",
	$year+1900, $month+1, $day, $hour, $min );

&backupcopy($_) foreach @files;
exit(0);

sub backupcopy {
	my($name)= @_;
	system("cp $sdir$name.txt $tdir$name$file.txt");
}

sub restorecopy {
	my($name)= @_;
	unless( -f "$sdir$name.txt" ){
		printf("$0: \"$sdir$name.txt\" not found!\n");
		return 0;
	}
	unless( -f "$tdir$name$file.txt" ){
		printf("$0: \"$tdir$name$file.txt\" not found!\n");
		return 0;
	}
	rename "$sdir$name.txt","$sdir$name.bak";
	system("cp $tdir$name$file.txt $sdir$name.txt");
}
