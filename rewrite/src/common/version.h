// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#ifndef _VERSION_H_
#define _VERSION_H_

#define ATHENA_MAJOR_VERSION	1	// Major Version
#define ATHENA_MINOR_VERSION	0	// Minor Version
#define ATHENA_REVISION			0	// Revision

#define ATHENA_RELEASE_FLAG		1	// 1=Develop,0=Stable
#define ATHENA_OFFICIAL_FLAG	1	// 1=Mod,0=Official

#define ATHENA_SERVER_NONE	0	// not defined
#define ATHENA_SERVER_LOGIN	1	// login server
#define ATHENA_SERVER_CHAR	2	// char server
#define ATHENA_SERVER_INTER	4	// inter server
#define ATHENA_SERVER_MAP	8	// map server

// ATHENA_MOD_VERSION�̓p�b�`�ԍ��ł��B
// ����͖����ɕς��Ȃ��Ă��C����������ς�����x�̈����ŁB
// �i����A�b�v���[�h�̓x�ɕύX����̂��ʓ|�Ǝv���邵�A��������
// �@���̍��ڂ��Q�Ƃ���l�����邩�ǂ����ŋ^�₾����B�j
// ���̒��x�̈����Ȃ̂ŁA�T�[�o�[�ɖ₢���킹�鑤���A�����܂Ŗڈ����x�̈�����
// ����܂�M�p���Ȃ����ƁB
// �Isnapshot�̎���A�傫�ȕύX���������ꍇ�͐ݒ肵�Ăق����ł��B
// C����̎d�l��A�ŏ���0��t�����8�i���ɂȂ�̂ŊԈႦ�Ȃ��ŉ������B
#define ATHENA_MOD_VERSION	1249	// mod version (patch No.)

#endif /* _VERSION_H_ */
