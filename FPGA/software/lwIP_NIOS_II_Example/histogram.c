/*
 *  TOPPERS ATK2
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *      Automotive Kernel Version 2
 *
 *  Copyright (C) 2006-2015 by Center for Embedded Computing Systems
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 *  Copyright (C) 2011-2015 by FUJI SOFT INCORPORATED, JAPAN
 *  Copyright (C) 2011-2013 by Spansion LLC, USA
 *  Copyright (C) 2011-2015 by NEC Communication Systems, Ltd., JAPAN
 *  Copyright (C) 2011-2015 by Panasonic Advanced Technology Development Co., Ltd., JAPAN
 *  Copyright (C) 2011-2014 by Renesas Electronics Corporation, JAPAN
 *  Copyright (C) 2011-2015 by Sunny Giken Inc., JAPAN
 *  Copyright (C) 2011-2015 by TOSHIBA CORPORATION, JAPAN
 *  Copyright (C) 2011-2015 by Witz Corporation
 *  Copyright (C) 2014-2015 by AISIN COMCRUISE Co., Ltd., JAPAN
 *  Copyright (C) 2014-2015 by eSOL Co.,Ltd., JAPAN
 *  Copyright (C) 2014-2015 by SCSK Corporation, JAPAN
 *
 *  ��L���쌠�҂́C�ȉ���(1)�`(4)�̏����𖞂����ꍇ�Ɍ���C�{�\�t�g�E�F
 *  �A�i�{�\�t�g�E�F�A�����ς������̂��܂ށD�ȉ������j���g�p�E�����E��
 *  �ρE�Ĕz�z�i�ȉ��C���p�ƌĂԁj���邱�Ƃ𖳏��ŋ�������D
 *  (1) �{�\�t�g�E�F�A���\�[�X�R�[�h�̌`�ŗ��p����ꍇ�ɂ́C��L�̒���
 *      ���\���C���̗��p��������щ��L�̖��ۏ؋K�肪�C���̂܂܂̌`�Ń\�[
 *      �X�R�[�h���Ɋ܂܂�Ă��邱�ƁD
 *  (2) �{�\�t�g�E�F�A���C���C�u�����`���ȂǁC���̃\�t�g�E�F�A�J���Ɏg
 *      �p�ł���`�ōĔz�z����ꍇ�ɂ́C�Ĕz�z�ɔ����h�L�������g�i���p
 *      �҃}�j���A���Ȃǁj�ɁC��L�̒��쌠�\���C���̗��p��������щ��L
 *      �̖��ۏ؋K����f�ڂ��邱�ƁD
 *  (3) �{�\�t�g�E�F�A���C�@��ɑg�ݍ��ނȂǁC���̃\�t�g�E�F�A�J���Ɏg
 *      �p�ł��Ȃ��`�ōĔz�z����ꍇ�ɂ́C���̂����ꂩ�̏����𖞂�����
 *      �ƁD
 *    (a) �Ĕz�z�ɔ����h�L�������g�i���p�҃}�j���A���Ȃǁj�ɁC��L�̒�
 *        �쌠�\���C���̗��p��������щ��L�̖��ۏ؋K����f�ڂ��邱�ƁD
 *    (b) �Ĕz�z�̌`�Ԃ��C�ʂɒ�߂���@�ɂ���āCTOPPERS�v���W�F�N�g��
 *        �񍐂��邱�ƁD
 *  (4) �{�\�t�g�E�F�A�̗��p�ɂ�蒼�ړI�܂��͊ԐړI�ɐ����邢���Ȃ鑹
 *      �Q������C��L���쌠�҂����TOPPERS�v���W�F�N�g��Ɛӂ��邱�ƁD
 *      �܂��C�{�\�t�g�E�F�A�̃��[�U�܂��̓G���h���[�U����̂����Ȃ闝
 *      �R�Ɋ�Â�����������C��L���쌠�҂����TOPPERS�v���W�F�N�g��
 *      �Ɛӂ��邱�ƁD
 *
 *  �{�\�t�g�E�F�A�́CAUTOSAR�iAUTomotive Open System ARchitecture�j�d
 *  �l�Ɋ�Â��Ă���D��L�̋����́CAUTOSAR�̒m�I���Y��������������̂�
 *  �͂Ȃ��DAUTOSAR�́CAUTOSAR�d�l�Ɋ�Â����\�t�g�E�F�A�����p�ړI�ŗ�
 *  �p����҂ɑ΂��āCAUTOSAR�p�[�g�i�[�ɂȂ邱�Ƃ����߂Ă���D
 *
 *  �{�\�t�g�E�F�A�́C���ۏ؂Œ񋟂���Ă�����̂ł���D��L���쌠�҂�
 *  ���TOPPERS�v���W�F�N�g�́C�{�\�t�g�E�F�A�Ɋւ��āC����̎g�p�ړI
 *  �ɑ΂���K�������܂߂āC�����Ȃ�ۏ؂��s��Ȃ��D�܂��C�{�\�t�g�E�F
 *  �A�̗��p�ɂ�蒼�ړI�܂��͊ԐړI�ɐ����������Ȃ鑹�Q�Ɋւ��Ă��C��
 *  �̐ӔC�𕉂�Ȃ��D
 *
 *  $Id: histogram.c 1801 2015-03-27 06:34:43Z t_ishikawa $
 */

/*
 *		���s���ԕ��z�W�v���W���[��
 */

#include <stdio.h>
#include "sys/alt_timestamp.h"
#include "histogram.h"
#include "echo.h"

/*
 *  ���s���ԕ��z�v���̐�
 */
#ifndef TNUM_HIST
#define TNUM_HIST		10
#endif /* TNUM_HIST */

alt_u32 timestamp_freq;

/*
 *  �^�[�Q�b�g�ˑ����Őݒ�ύX���邽�߂̃}�N��
 */
#ifndef HISTTIM                     /* ���s���Ԍv���p�̎����̃f�[�^�^ */
#define HISTTIM			SystemTimeUsType
#endif /* HISTTIM */

#ifndef HIST_GET_TIM                /* ���s���Ԍv���p�̌��ݎ����̎擾 */
#define HIST_GET_TIM(p_time)	((void) get_utm(p_time))
#endif /* HIST_GET_TIM */

#ifndef HIST_CONV_TIM               /* �����̍�������s���Ԃւ̕ϊ� */
#define HIST_CONV_TIM(time)		((uint32) (time))
#endif /* HIST_CONV_TIM */

//#ifndef HIST_BM_HOOK                /* ���s���Ԍv�����O�ɍs���ׂ����� */
//#define HIST_BM_HOOK()
//#endif /* HIST_BM_HOOK */

void get_utm(HISTTIM *p_time)
{
	//*p_time = (HISTTIM) ((double)alt_timestamp() / (double)timestamp_freq * (double)1000000);
	*p_time = (HISTTIM)alt_timestamp();
	//printf("%d\n", *p_time);
}

/*
 *  ���s���ԕ��z�v���Ǘ��u���b�N
 */
typedef struct histogram_control_block {
	HISTTIM	begin_time;             /* �v���J�n���� */
	alt_u32	maxval;                 /* ���z���L�^����ő厞�� */
	alt_u32	*histarea;              /* ���z���L�^���郁�����̈� */
	alt_u32	over;                   /* �ő厞�Ԃ𒴂����x�� */
	alt_u32	under;                  /* ���Ԃ̋t�]���^����x�� */
	boolean	initialized;            /* �������ς݂��ǂ����̃t���O */
} HISTCB;

/*
 *  ���s���ԕ��z�v���Ǘ��u���b�N�̃G���A
 */
HISTCB histcb_table[TNUM_HIST];

/*
 *  ���s���ԕ��z�v��ID�̍ŏ��l�ƍő�l
 */
#define TMIN_HISTID		1
#define TMAX_HISTID		((TMIN_HISTID + TNUM_HIST) - 1)

/*
 *  ���s���ԕ��z�v���̏�����
 */
boolean
init_hist(ObjectIDType histid, uint32 maxval, uint32 *histarea)
{
	HISTCB	*p_histcb;
	uint32	i;
	boolean ret = TRUE;

	/* �s���ȕ��z�v��ID�̏ꍇFALSE��Ԃ� */
	if ((histid < TMIN_HISTID) || (histid > TMAX_HISTID)) {
		ret = FALSE;
	}
	else {
		p_histcb = &(histcb_table[histid - TMIN_HISTID]);

		for (i = 0U; i <= maxval; i++) {
			histarea[i] = 0U;
		}
		p_histcb->maxval = maxval;
		p_histcb->histarea = histarea;
		p_histcb->over = 0U;
		p_histcb->under = 0U;
		p_histcb->initialized = TRUE;
	}
	return(ret);
}

/*
 *  ���s���Ԍv���̊J�n
 */
boolean
begin_measure(ObjectIDType histid)
{
	HISTCB	*p_histcb;
	boolean ret = TRUE;
	if((int)histid % 500 == 1) {
		alt_timestamp_start();
	}

	/* �s���ȕ��z�v��ID�̏ꍇFALSE��Ԃ� */
	if ((histid < TMIN_HISTID) || (histid > TMAX_HISTID)) {
		ret = FALSE;
	}
	else {
		p_histcb = &(histcb_table[histid - TMIN_HISTID]);

		/* ���������Ă��Ȃ��ꍇFALSE��Ԃ� */
		if (p_histcb->initialized == FALSE)	{
			ret = FALSE;
		}
		else {
			//HIST_BM_HOOK();
			HIST_GET_TIM(&(p_histcb->begin_time));
		}
	}
	return(ret);
}

/*
 *  ���s���Ԍv���̏I��
 */
boolean
end_measure(ObjectIDType histid)
{
	HISTCB	*p_histcb;
	HISTTIM	end_time;
	uint32	val;
	boolean ret = TRUE;

	HIST_GET_TIM(&end_time);

	/* �s���ȕ��z�v��ID�̏ꍇFALSE��Ԃ� */
	if ((histid < TMIN_HISTID) || (histid > TMAX_HISTID)) {
		ret = FALSE;
	}
	else {
		p_histcb = &(histcb_table[histid - TMIN_HISTID]);

		/* ���������Ă��Ȃ��ꍇFALSE��Ԃ� */
		if (p_histcb->initialized == FALSE)	{
			ret = FALSE;
		}
		else {
			//val = HIST_CONV_TIM(end_time - p_histcb->begin_time);
			val = (uint32) ((double)HIST_CONV_TIM(end_time - p_histcb->begin_time) / (double)timestamp_freq * (double)1000000);
			//printf("timediff: %d\n", val);
			if (val <= p_histcb->maxval) {
				p_histcb->histarea[val]++;
			}
			else if (val <= ((uint32) 0x7fffffff)) {
				p_histcb->over++;
			}
			else {
				p_histcb->under++;
			}
		}
	}
	return(ret);
}

/*
 *  ���s���ԕ��z�v���̕\��
 */
boolean
print_hist(ObjectIDType histid)
{
	HISTCB	*p_histcb;
	int		i;
	boolean ret = TRUE;

	/* �s���ȕ��z�v��ID�̏ꍇFALSE��Ԃ� */
	if ((histid < TMIN_HISTID) || (histid > TMAX_HISTID)) {
		ret = FALSE;
	}
	else {
		p_histcb = &(histcb_table[histid - TMIN_HISTID]);

		/* ���������Ă��Ȃ��ꍇFALSE��Ԃ� */
		if (p_histcb->initialized == FALSE)	{
			ret = FALSE;
		}
		else {
			for (i = 0U; i <= (int)p_histcb->maxval; i++) {
				if ((int)p_histcb->histarea[i] > 0U) {
#ifndef MEASURE_100_NANO
					printf("%d : %d\n", i, (int)p_histcb->histarea[i]);
#else
					printf("%d.%d : %d\n", i / 10, i % 10, (int)p_histcb->histarea[i]);
#endif /* MEASURE_100_NANO */
				}
			}
			if (p_histcb->over > 0U) {
				printf("> %d : %d\n", (int)p_histcb->maxval, (int)p_histcb->over);
			}
			if (p_histcb->under > 0U) {
				printf("> 0x7fffffff : %d\n", (int)p_histcb->under);
			}
		}
	}

	return(ret);
}
