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
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 *
 *  本ソフトウェアは，AUTOSAR（AUTomotive Open System ARchitecture）仕
 *  様に基づいている．上記の許諾は，AUTOSARの知的財産権を許諾するもので
 *  はない．AUTOSARは，AUTOSAR仕様に基づいたソフトウェアを商用目的で利
 *  用する者に対して，AUTOSARパートナーになることを求めている．
 *
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 *
 *  $Id: histogram.c 1801 2015-03-27 06:34:43Z t_ishikawa $
 */

/*
 *		実行時間分布集計モジュール
 */

#include <stdio.h>
#include "sys/alt_timestamp.h"
#include "histogram.h"
#include "echo.h"

/*
 *  実行時間分布計測の数
 */
#ifndef TNUM_HIST
#define TNUM_HIST		10
#endif /* TNUM_HIST */

alt_u32 timestamp_freq;

/*
 *  ターゲット依存部で設定変更するためのマクロ
 */
#ifndef HISTTIM                     /* 実行時間計測用の時刻のデータ型 */
#define HISTTIM			SystemTimeUsType
#endif /* HISTTIM */

#ifndef HIST_GET_TIM                /* 実行時間計測用の現在時刻の取得 */
#define HIST_GET_TIM(p_time)	((void) get_utm(p_time))
#endif /* HIST_GET_TIM */

#ifndef HIST_CONV_TIM               /* 時刻の差から実行時間への変換 */
#define HIST_CONV_TIM(time)		((uint32) (time))
#endif /* HIST_CONV_TIM */

//#ifndef HIST_BM_HOOK                /* 実行時間計測直前に行うべき処理 */
//#define HIST_BM_HOOK()
//#endif /* HIST_BM_HOOK */

void get_utm(HISTTIM *p_time)
{
	//*p_time = (HISTTIM) ((double)alt_timestamp() / (double)timestamp_freq * (double)1000000);
	*p_time = (HISTTIM)alt_timestamp();
	//printf("%d\n", *p_time);
}

/*
 *  実行時間分布計測管理ブロック
 */
typedef struct histogram_control_block {
	HISTTIM	begin_time;             /* 計測開始時刻 */
	alt_u32	maxval;                 /* 分布を記録する最大時間 */
	alt_u32	*histarea;              /* 分布を記録するメモリ領域 */
	alt_u32	over;                   /* 最大時間を超えた度数 */
	alt_u32	under;                  /* 時間の逆転が疑われる度数 */
	boolean	initialized;            /* 初期化済みかどうかのフラグ */
} HISTCB;

/*
 *  実行時間分布計測管理ブロックのエリア
 */
HISTCB histcb_table[TNUM_HIST];

/*
 *  実行時間分布計測IDの最小値と最大値
 */
#define TMIN_HISTID		1
#define TMAX_HISTID		((TMIN_HISTID + TNUM_HIST) - 1)

/*
 *  実行時間分布計測の初期化
 */
boolean
init_hist(ObjectIDType histid, uint32 maxval, uint32 *histarea)
{
	HISTCB	*p_histcb;
	uint32	i;
	boolean ret = TRUE;

	/* 不正な分布計測IDの場合FALSEを返す */
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
 *  実行時間計測の開始
 */
boolean
begin_measure(ObjectIDType histid)
{
	HISTCB	*p_histcb;
	boolean ret = TRUE;
	if((int)histid % 500 == 1) {
		alt_timestamp_start();
	}

	/* 不正な分布計測IDの場合FALSEを返す */
	if ((histid < TMIN_HISTID) || (histid > TMAX_HISTID)) {
		ret = FALSE;
	}
	else {
		p_histcb = &(histcb_table[histid - TMIN_HISTID]);

		/* 初期化していない場合FALSEを返す */
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
 *  実行時間計測の終了
 */
boolean
end_measure(ObjectIDType histid)
{
	HISTCB	*p_histcb;
	HISTTIM	end_time;
	uint32	val;
	boolean ret = TRUE;

	HIST_GET_TIM(&end_time);

	/* 不正な分布計測IDの場合FALSEを返す */
	if ((histid < TMIN_HISTID) || (histid > TMAX_HISTID)) {
		ret = FALSE;
	}
	else {
		p_histcb = &(histcb_table[histid - TMIN_HISTID]);

		/* 初期化していない場合FALSEを返す */
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
 *  実行時間分布計測の表示
 */
boolean
print_hist(ObjectIDType histid)
{
	HISTCB	*p_histcb;
	int		i;
	boolean ret = TRUE;

	/* 不正な分布計測IDの場合FALSEを返す */
	if ((histid < TMIN_HISTID) || (histid > TMAX_HISTID)) {
		ret = FALSE;
	}
	else {
		p_histcb = &(histcb_table[histid - TMIN_HISTID]);

		/* 初期化していない場合FALSEを返す */
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
