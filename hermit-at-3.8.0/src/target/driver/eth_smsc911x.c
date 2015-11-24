#include <platform.h>
#include <herrno.h>
#include <io.h>
#include <string.h>

#include <net/eth.h>
#include <net/eth_util.h>
#include <delay.h>
#include <mac.h>

#include "eth_smsc911x.h"

#if defined(DEBUG_ETH)
#define DEBUG_SMSC911X
#endif

#if defined(DEBUG_SMSC911X)
#define DEBUG_EN_TRACE
#define DEBUG_EN_WARNING
#define DEBUG_EN_TESTCODE
#endif

#if defined(DEBUG_EN_TRACE)
#define SMSC_TRACE(msg, args...) hprintf("SMSC: " msg "\n", ## args)
#else
#define SMSC_TRACE(msg, args...)
#endif

#if defined(DEBUG_EN_WARNING)
#define SMSC_WARNING(msg, args...) SMSC_TRACE(msg, ## args)
#else
#define SMSC_WARNING(msg, args...)
#endif

#define SMSC_ASSERT(condition)

#define TRUE	((BOOLEAN)1)
#define FALSE	((BOOLEAN)0)

#define HIBYTE(word)  ((BYTE)(((WORD)(word))>>8))
#define LOBYTE(word)  ((BYTE)(((WORD)(word))&0x00FFU))
#define HIWORD(dWord) ((WORD)(((DWORD)(dWord))>>16))
#define LOWORD(dWord) ((WORD)(((DWORD)(dWord))&0x0000FFFFUL))

#define ENODEV 1

PRIVATE_DATA _privateData;
PPRIVATE_DATA privateData = &_privateData;
PLATFORM_DATA _platformData;
PPLATFORM_DATA platformData = &_platformData;

unsigned char *EthTxBuffer = (unsigned char *)0x80000000;
unsigned char *EthRxBuffer = (unsigned char *)0x80100000;

#define MIN_PACKET_SIZE 60

#define OLD_REGISTERS(privData)                    \
	(((privData->dwIdRev&0x0000FFFFUL)==0UL)&& \
	 ((privData->dwFpgaRev)>=0x01)&&           \
	 ((privData->dwFpgaRev)<=0x25))

static struct platform_info *pinfo = &platform_info;

inline void
Platform_SetRegDW(DWORD dwLanBase,
		  DWORD dwOffset,
		  DWORD dwVal)
{
#if REVA
	*(volatile u16 *)((dwLanBase) + ((dwOffset) << 1)) = ((dwVal) & 0xffff);
	*(volatile u16 *)((dwLanBase) + (((dwOffset) + 2) << 1)) = (((dwVal)>>16) & 0xffff);
#else
	(*(volatile DWORD *)(dwLanBase+dwOffset))=dwVal;
#endif
}

inline DWORD
Platform_GetRegDW(DWORD dwLanBase,
		  DWORD dwOffset)
{
#if REVA
	u32 val;
	val =
		(*(volatile u16 *)((dwLanBase) + ((dwOffset)<<1))) |
		((*(volatile u16 *)((dwLanBase) + (((dwOffset) + 2)<<1)))<<16);
	return val;
#else
	return (*(volatile DWORD *)(dwLanBase+dwOffset));
#endif
}

DWORD
Platform_Initialize(PPLATFORM_DATA platformData,
		    DWORD dwLanBase,DWORD dwBusWidth)
{
	struct net_device *dev = pinfo->net_dev;
	DWORD mode=0;
	DWORD test=0;
	platformData->dwBitWidth=0;
	platformData->dwIdRev=0;
	SMSC_TRACE("--> Platform_Initialize()");

	if (!dev)
		return -1;

	if(dwLanBase==0x0UL) {
		dwLanBase=dev->base_addr;
		SMSC_TRACE("Using dwLanBase==%p",dwLanBase);
	}

	if(dwLanBase!=dev->base_addr) {
		SMSC_WARNING("PlatformInitialize: resetting dwLanBase from %p to %p", dwLanBase, dev->base_addr);
		dwLanBase=dev->base_addr;
	}

#if defined(CONFIG_SUZAKU)
	/* need to access register before it'll sets endian. */
	*(volatile unsigned long *) (dwLanBase+BYTE_TEST);
	/* Endian Register */
	*(volatile unsigned long *) (dwLanBase+ENDIAN) = 0xffffffff;
#endif

	switch(dwBusWidth) {
	case 16:
		mode=2;
		platformData->dwBitWidth=16;
		test = Platform_GetRegDW(dwLanBase, BYTE_TEST);
		if(test==0x87654321) {
			SMSC_TRACE(" 16 bit mode assigned and verified");
		} else {
			SMSC_WARNING(" failed to verify assigned 16 bit mode");
			dwLanBase=0;
		};break;
	case 32:
		mode=1;
		platformData->dwBitWidth=32;
		test = Platform_GetRegDW(dwLanBase, BYTE_TEST);
		if(test==0x87654321) {
			SMSC_TRACE(" 32 bit mode assigned and verified");
		} else {
			SMSC_WARNING(" failed to verify assigned 32 bit mode");
			dwLanBase=0;
		};break;
	default:
		mode=1;
		platformData->dwBitWidth=32;
		{
			WORD dummy=0;
			test = Platform_GetRegDW(dwLanBase, BYTE_TEST);
			dummy = Platform_GetRegDW(dwLanBase, BYTE_TEST);
			if(test==0x87654321UL) {
				SMSC_TRACE(" 32 bit mode detected");
			} else {
				SMSC_TRACE(" 32 bit mode not detected, test=%p",test);
				platformData->dwBitWidth=16;

				test = Platform_GetRegDW(dwLanBase, BYTE_TEST);
				if(test==0x87654321UL) {
					SMSC_TRACE(" 16 bit mode detected");
					mode=2;
				} else {
					SMSC_WARNING(" neither 16 nor 32 bit mode detected.");
					dwLanBase=0;
				}
			}
			dummy=dummy;/* make lint happy */
		};break;
	}

	if(dwLanBase!=0) {
		DWORD idRev = Platform_GetRegDW(dwLanBase, ID_REV);
		platformData->dwIdRev=idRev;
		switch(idRev&0xFFFF0000) {
		case 0x01180000UL:
		case 0x01170000UL:
		case 0x01120000UL:
			switch(mode) {
			case 1:/* 32 bit mode */
				platformData->dwBitWidth=32;
				break;
			case 2:/* 16 bit mode */
				platformData->dwBitWidth=16;
				break;
			default:
				break;/* make lint happy */
			};
			break;
		default:
			break;/* make lint happy */
		}
	}

	SMSC_TRACE("<-- Platform_Initialize()");
	return dwLanBase;
}

#if defined(CONFIG_SUZAKU)
static inline unsigned long
__dword_swap(unsigned long v)
{
	unsigned long _v;
	_v =  ((v & 0x000000ff) << 24);
	_v |= ((v & 0x0000ff00) << 8);
	_v |= ((v & 0x00ff0000) >> 8);
	_v |= ((v & 0xff000000) >> 24);
	return _v;
}
#endif

static void
Platform_WriteFifo(DWORD dwLanBase,
		   DWORD *pdwBuf,
		   DWORD dwDwordCount)
{
	volatile DWORD *pdwReg;
#if !REVA
	pdwReg = (volatile DWORD *)(dwLanBase+TX_DATA_FIFO);
#endif
	while(dwDwordCount)
	{
#if defined(CONFIG_SUZAKU)
		*pdwReg = __dword_swap(*pdwBuf++);
#else
#if !REVA
		*pdwReg = *pdwBuf++;
#else
		/*a500 rev.a*/
		Platform_SetRegDW(dwLanBase, TX_DATA_FIFO, *pdwBuf++);

#endif
#endif
		dwDwordCount--;
	}
}

static void
Platform_ReadFifo(DWORD dwLanBase,
		  DWORD *pdwBuf,
		  DWORD dwDwordCount)
{
#if !REVA
	const volatile DWORD * const pdwReg =
		(const volatile DWORD * const)(dwLanBase+RX_DATA_FIFO);
#else
	/*a500 rev.a*/
	const volatile DWORD * const pdwReg =
		(const volatile DWORD * const)(dwLanBase+(RX_DATA_FIFO<<1));
#endif

	while (dwDwordCount)
	{
#if defined(DEBUG_EN_TESTCODE)
		if(!(Lan_GetRegDW(RX_FIFO_INF) & RX_FIFO_INF_RXDUSED_)){
			SMSC_WARNING("RX_DATA_FIFO underrun\n");
		}
#endif

#if defined(CONFIG_SUZAKU)
		*pdwBuf++ = __dword_swap(*pdwReg);
#else
#if !REVA
		*pdwBuf++ = *pdwReg;
#else
		*pdwBuf++ = Platform_GetRegDW(dwLanBase, RX_DATA_FIFO);
#endif
#endif
		dwDwordCount--;
	}
}

static BOOLEAN
MacNotBusy(PPRIVATE_DATA privateData, int dummy)
{
	int i=0;

	/* wait for MAC not busy, w/ timeout */
	for(i=0;i<40;i++)
	{
		if((Lan_GetRegDW(MAC_CSR_CMD) & MAC_CSR_CMD_CSR_BUSY_)==(0UL)) {
			return TRUE;
		}
	}
	SMSC_WARNING("timeout waiting for MAC not BUSY. MAC_CSR_CMD = %p",
		Lan_GetRegDW(MAC_CSR_CMD));
	return FALSE;
}

static DWORD
Mac_GetRegDW(PPRIVATE_DATA privateData,DWORD dwRegOffset)
{
	DWORD result=0xFFFFFFFFUL;
	DWORD dwTemp=0;

	/* wait until not busy */
	if (Lan_GetRegDW(MAC_CSR_CMD) & MAC_CSR_CMD_CSR_BUSY_)
	{
		SMSC_WARNING("Mac_GetRegDW() failed, MAC already busy at entry");
		goto DONE;
	}

	/* send the MAC Cmd w/ offset */
	Lan_SetRegDW(MAC_CSR_CMD,
		((dwRegOffset & 0x000000FFUL) |
		MAC_CSR_CMD_CSR_BUSY_ | MAC_CSR_CMD_R_NOT_W_));
	dwTemp=Lan_GetRegDW(BYTE_TEST);/* to flush previous write */
	dwTemp=dwTemp;

	/* wait for the read to happen, w/ timeout */
	if (!MacNotBusy(privateData,0))
	{
		SMSC_WARNING("Mac_GetRegDW() failed, waiting for MAC not busy after read");
		goto DONE;
	} else {
	  /* finally, return the read data */
		result=Lan_GetRegDW(MAC_CSR_DATA);
	}
DONE:
	return result;
}

static void
Mac_SetRegDW(PPRIVATE_DATA privateData,DWORD dwRegOffset,DWORD dwVal)
{
	DWORD dwTemp=0;

	if (Lan_GetRegDW(MAC_CSR_CMD) & MAC_CSR_CMD_CSR_BUSY_)
	{
		SMSC_WARNING("Mac_SetRegDW() failed, MAC already busy at entry");
		goto DONE;
	}

	/* send the data to write */
	Lan_SetRegDW(MAC_CSR_DATA,dwVal);

	/* do the actual write */
	Lan_SetRegDW(MAC_CSR_CMD,((dwRegOffset & 0x000000FFUL) | MAC_CSR_CMD_CSR_BUSY_));
	dwTemp=Lan_GetRegDW(BYTE_TEST);/* force flush of previous write */
	dwTemp=dwTemp;

	/* wait for the write to complete, w/ timeout */
	if (!MacNotBusy(privateData,0))
	{
		SMSC_WARNING("Mac_SetRegDW() failed, waiting for MAC not busy after write");
	}
DONE:
	return;
}

static WORD
Phy_GetRegW(PPRIVATE_DATA privateData,DWORD dwRegIndex)
{
	DWORD dwAddr=0;
	int i=0;
	WORD result=0xFFFFU;

	SMSC_ASSERT(privateData!=NULL);
	SMSC_ASSERT(privateData->LanInitialized==TRUE);
	SMSC_ASSERT(Vl_CheckLock(&(privateData->MacPhyLock),0));

	/* confirm MII not busy */
	if ((Mac_GetRegDW(privateData, MII_ACC) & MII_ACC_MII_BUSY_) != 0UL)
	{
		SMSC_WARNING("MII is busy in Phy_GetRegW???");
		result=0;
		goto DONE;
	}

	/* set the address, index & direction (read from PHY) */
	dwAddr = ((privateData->dwPhyAddress&0x1FUL)<<11) | ((dwRegIndex & 0x1FUL)<<6);
	Mac_SetRegDW(privateData, MII_ACC, dwAddr);

	/* wait for read to complete w/ timeout */
	for(i=0;i<100;i++) {
		/* see if MII is finished yet */
		if ((Mac_GetRegDW(privateData, MII_ACC) & MII_ACC_MII_BUSY_) == 0UL)
		{
			/* get the read data from the MAC & return i */
			result=((WORD)Mac_GetRegDW(privateData, MII_DATA));
			goto DONE;
		}
	}
	SMSC_WARNING("timeout waiting for MII write to finish");

DONE:
	return result;
}

static void
Phy_SetRegW(PPRIVATE_DATA privateData,DWORD dwRegIndex,WORD wVal)
{
	DWORD dwAddr=0;
	int i=0;

	SMSC_ASSERT(privateData!=NULL);
	SMSC_ASSERT(privateData->LanInitialized==TRUE);

	SMSC_ASSERT(Vl_CheckLock(&(privateData->MacPhyLock),0));

	if(dwRegIndex==0) {
		if((wVal&0x1200)==0x1200) {
			privateData->wLastADVatRestart=privateData->wLastADV;
		}
	}
	if(dwRegIndex==4) {
		privateData->wLastADV=wVal;
	}

	/* confirm MII not busy */
	if ((Mac_GetRegDW(privateData, MII_ACC) & MII_ACC_MII_BUSY_) != 0UL)
	{
		SMSC_WARNING("MII is busy in Phy_SetRegW???");
		goto DONE;
	}

	/* put the data to write in the MAC */
	Mac_SetRegDW(privateData, MII_DATA, (DWORD)wVal);

	/* set the address, index & direction (write to PHY) */
	dwAddr = ((privateData->dwPhyAddress&0x1FUL)<<11) | ((dwRegIndex & 0x1FUL)<<6) | MII_ACC_MII_WRITE_;
	Mac_SetRegDW(privateData, MII_ACC, dwAddr);

	/* wait for write to complete w/ timeout */
	for(i=0;i<100;i++) {
		/* see if MII is finished yet */
		if ((Mac_GetRegDW(privateData, MII_ACC) & MII_ACC_MII_BUSY_) == 0UL)
		{
			goto DONE;
		}
	}
	SMSC_WARNING("timeout waiting for MII write to finish");
DONE:
	return;
}

static DWORD
Phy_LBT_GetTxStatus(PPRIVATE_DATA privateData)
{
	DWORD result=Lan_GetRegDW(TX_FIFO_INF);
	if(OLD_REGISTERS(privateData)) {
		result&=TX_FIFO_INF_TSFREE_;
		if(result!=0x00800000UL) {
			result=Lan_GetRegDW(TX_STATUS_FIFO);
		} else {
			result=0;
		}
	} else {
		result&=TX_FIFO_INF_TSUSED_;
		if(result!=0x00000000UL) {
			result=Lan_GetRegDW(TX_STATUS_FIFO);
		} else {
			result=0;
		}
	}
	return result;
}

#if defined(DEBUG_EN_TESTCODE)
static BOOLEAN
Phy_TransmitTestPacket(PPRIVATE_DATA privateData)
{
	BYTE LoopBackTxPacket[70];
	BOOLEAN result=FALSE;
	DWORD dwLoopCount=0;
	DWORD dwTxCmdA=0;
	DWORD dwTxCmdB=0;
	DWORD dwStatus=0;

	/* write Tx Packet to 118 */
	dwTxCmdA=
		((((DWORD)(LoopBackTxPacket))&0x03UL)<<16) | /* DWORD alignment adjustment */
		TX_CMD_A_INT_FIRST_SEG_ | TX_CMD_A_INT_LAST_SEG_ |
		((DWORD)(MIN_PACKET_SIZE));
	dwTxCmdB=
		(((DWORD)(MIN_PACKET_SIZE))<<16) |
		((DWORD)(MIN_PACKET_SIZE));

	Lan_SetRegDW(TX_DATA_FIFO,dwTxCmdA);
	Lan_SetRegDW(TX_DATA_FIFO,dwTxCmdB);
	Platform_WriteFifo(
		privateData->dwLanBase,
		(DWORD *)(((DWORD)(LoopBackTxPacket))&0xFFFFFFFCUL),
		(((DWORD)(MIN_PACKET_SIZE))+3+
		(((DWORD)(LoopBackTxPacket))&0x03UL))>>2);

	/* wait till transmit is done */
	dwLoopCount=60;
	while((dwLoopCount>0) &&((dwStatus=Phy_LBT_GetTxStatus(privateData))==0)) {
		udelay(5);
		dwLoopCount--;
	}
	if(dwStatus==0) {
		SMSC_WARNING("Failed to Transmit during Packet Test");
		goto DONE;
	}
	if(dwStatus&0x00008000UL) {
		SMSC_WARNING("Transmit encountered errors during Packet Test");
		goto DONE;
	}
DONE:
	return result;
}
#endif

static void
Phy_GetLinkMode(PPRIVATE_DATA privateData)
{
	DWORD result=LINK_OFF;
	WORD wRegVal=0;
	WORD wRegBSR=Phy_GetRegW(
		privateData,
		PHY_BSR);
	privateData->dwLinkSettings=LINK_OFF;
	if(wRegBSR&PHY_BSR_LINK_STATUS_) {
		wRegVal=Phy_GetRegW(
			privateData,
			PHY_BCR);
		if(wRegVal&PHY_BCR_AUTO_NEG_ENABLE_) {
			DWORD linkSettings=LINK_AUTO_NEGOTIATE;
			WORD wRegADV=privateData->wLastADVatRestart;
			WORD wRegLPA=Phy_GetRegW(
				privateData,
				PHY_ANEG_LPA);
			if(wRegADV&PHY_ANEG_ADV_ASYMP_) {
				linkSettings|=LINK_ASYMMETRIC_PAUSE;
			}
			if(wRegADV&PHY_ANEG_ADV_SYMP_) {
				linkSettings|=LINK_SYMMETRIC_PAUSE;
			}
			if(wRegADV&PHY_ANEG_LPA_100FDX_) {
				linkSettings|=LINK_SPEED_100FD;
			}
			if(wRegADV&PHY_ANEG_LPA_100HDX_) {
				linkSettings|=LINK_SPEED_100HD;
			}
			if(wRegADV&PHY_ANEG_LPA_10FDX_) {
				linkSettings|=LINK_SPEED_10FD;
			}
			if(wRegADV&PHY_ANEG_LPA_10HDX_) {
				linkSettings|=LINK_SPEED_10HD;
			}
			privateData->dwLinkSettings=linkSettings;
			wRegLPA&=wRegADV;
			if(wRegLPA&PHY_ANEG_LPA_100FDX_) {
				result=LINK_SPEED_100FD;
			} else if(wRegLPA&PHY_ANEG_LPA_100HDX_) {
				result=LINK_SPEED_100HD;
			} else if(wRegLPA&PHY_ANEG_LPA_10FDX_) {
				result=LINK_SPEED_10FD;
			} else if(wRegLPA&PHY_ANEG_LPA_10HDX_) {
				result=LINK_SPEED_10HD;
			}
		} else {
			if(wRegVal&PHY_BCR_SPEED_SELECT_) {
				if(wRegVal&PHY_BCR_DUPLEX_MODE_) {
					privateData->dwLinkSettings=result=LINK_SPEED_100FD;
				} else {
					privateData->dwLinkSettings=result=LINK_SPEED_100HD;
				}
			} else {
				if(wRegVal&PHY_BCR_DUPLEX_MODE_) {
					privateData->dwLinkSettings=result=LINK_SPEED_10FD;
				} else {
					privateData->dwLinkSettings=result=LINK_SPEED_10HD;
				}
			}
		}
	}
	privateData->dwLinkSpeed=result;
}

static int
Phy_UpdateLinkMode(PPRIVATE_DATA privateData)
{
	Phy_GetLinkMode(privateData);

	if(privateData->dwLinkSpeed!=LINK_OFF) {
		DWORD dwRegVal=0;
		switch(privateData->dwLinkSpeed) {
		case LINK_SPEED_10HD:
			SMSC_TRACE("Link is now UP at 10Mbps HD");
			break;
		case LINK_SPEED_10FD:
			SMSC_TRACE("Link is now UP at 10Mbps FD");
			break;
		case LINK_SPEED_100HD:
			SMSC_TRACE("Link is now UP at 100Mbps HD");
			break;
		case LINK_SPEED_100FD:
			SMSC_TRACE("Link is now UP at 100Mbps FD");
			break;
		default:
			SMSC_WARNING("Link is now UP at Unknown Link Speed, dwLinkSpeed=0x%08lX",
				     privateData->dwLinkSpeed);
			break;
		}

		dwRegVal=Mac_GetRegDW(privateData,MAC_CR);
		dwRegVal&=~(MAC_CR_FDPX_|MAC_CR_RCVOWN_);
		switch(privateData->dwLinkSpeed) {
		case LINK_SPEED_10HD:
		case LINK_SPEED_100HD:
			dwRegVal|=MAC_CR_RCVOWN_;
			break;
		case LINK_SPEED_10FD:
		case LINK_SPEED_100FD:
			dwRegVal|=MAC_CR_FDPX_;
			break;
		default:
			break;/* make lint happy */
		}

		Mac_SetRegDW(privateData,MAC_CR,dwRegVal);

		if(privateData->dwLinkSettings&LINK_AUTO_NEGOTIATE) {
			WORD linkPartner=0;
			WORD localLink=0;
			localLink=Phy_GetRegW(privateData,4);
			linkPartner=Phy_GetRegW(privateData,5);

			Mac_SetRegDW(privateData,FLOW,0UL);
			Lan_ClrBitsDW(AFC_CFG,0x0000000FUL);

			SMSC_TRACE("LAN911x: %s,%s,%s,%s,%s,%s",
				   (localLink&PHY_ANEG_ADV_ASYMP_)?"ASYMP":"     ",
				   (localLink&PHY_ANEG_ADV_SYMP_)?"SYMP ":"     ",
				   (localLink&PHY_ANEG_ADV_100F_)?"100FD":"     ",
				   (localLink&PHY_ANEG_ADV_100H_)?"100HD":"     ",
				   (localLink&PHY_ANEG_ADV_10F_)?"10FD ":"     ",
				   (localLink&PHY_ANEG_ADV_10H_)?"10HD ":"     ");

			SMSC_TRACE("Partner: %s,%s,%s,%s,%s,%s",
				   (linkPartner&PHY_ANEG_LPA_ASYMP_)?"ASYMP":"     ",
				   (linkPartner&PHY_ANEG_LPA_SYMP_)?"SYMP ":"     ",
				   (linkPartner&PHY_ANEG_LPA_100FDX_)?"100FD":"     ",
				   (linkPartner&PHY_ANEG_LPA_100HDX_)?"100HD":"     ",
				   (linkPartner&PHY_ANEG_LPA_10FDX_)?"10FD ":"     ",
				   (linkPartner&PHY_ANEG_LPA_10HDX_)?"10HD ":"     ");
		} else {
			switch(privateData->dwLinkSpeed) {
			case LINK_SPEED_10HD:
			case LINK_SPEED_100HD:
				Mac_SetRegDW(privateData,FLOW,0x0UL);
				Lan_SetBitsDW(AFC_CFG,0x0000000FUL);
				break;
			default:
				Mac_SetRegDW(privateData,FLOW,0x0UL);
				Lan_ClrBitsDW(AFC_CFG,0x0000000FUL);
				break;
			}
		}
	} else {
		SMSC_TRACE("Link is now DOWN");
		Mac_SetRegDW(privateData,FLOW,0UL);
		Lan_ClrBitsDW(AFC_CFG,0x0000000FUL);

		return -1;
	}
	return 0;
}

static int
Phy_CheckLink(unsigned long ptr)
{
	PPRIVATE_DATA privateData=(PPRIVATE_DATA)ptr;
	if(privateData==NULL) {
		SMSC_WARNING("Phy_CheckLink(ptr==0)");
		return -1;
	}

	/* must call this twice */
	return Phy_UpdateLinkMode(privateData);
}

static int
Phy_SetLink(PPRIVATE_DATA privateData,DWORD dwLinkRequest)
{
	if(dwLinkRequest&LINK_AUTO_NEGOTIATE) {
		WORD wTemp;
		wTemp=Phy_GetRegW(privateData,
			PHY_ANEG_ADV);
		wTemp&=~PHY_ANEG_ADV_PAUSE_;
		wTemp&=~PHY_ANEG_ADV_SPEED_;
		if(dwLinkRequest&LINK_SPEED_10HD) {
			wTemp|=PHY_ANEG_ADV_10H_;
		}
		if(dwLinkRequest&LINK_SPEED_10FD) {
			wTemp|=PHY_ANEG_ADV_10F_;
		}
		if(dwLinkRequest&LINK_SPEED_100HD) {
			wTemp|=PHY_ANEG_ADV_100H_;
		}
		if(dwLinkRequest&LINK_SPEED_100FD) {
			wTemp|=PHY_ANEG_ADV_100F_;
		}
		Phy_SetRegW(privateData,PHY_ANEG_ADV,wTemp);

		/* begin to establish link */
		privateData->dwRemoteFaultCount=0;
		Phy_SetRegW(privateData,
			PHY_BCR,
			PHY_BCR_AUTO_NEG_ENABLE_|
			PHY_BCR_RESTART_AUTO_NEG_);

		{
			int timeout=5000;
			while(timeout--){
				DWORD reg = Phy_GetRegW(privateData, PHY_BSR);
				if(reg & 0x20) break;
				mdelay(1);
				timeout--;
			}
			if(timeout <=0) return -1;
		}
	} else {
		WORD wTemp=0;
		if(dwLinkRequest&(LINK_SPEED_100FD)) {
			dwLinkRequest=LINK_SPEED_100FD;
		} else if(dwLinkRequest&(LINK_SPEED_100HD)) {
			dwLinkRequest=LINK_SPEED_100HD;
		} else if(dwLinkRequest&(LINK_SPEED_10FD)) {
			dwLinkRequest=LINK_SPEED_10FD;
		} else if(dwLinkRequest&(LINK_SPEED_10HD)) {
			dwLinkRequest=LINK_SPEED_10HD;
		}
		if(dwLinkRequest&(LINK_SPEED_10FD|LINK_SPEED_100FD)) {
			wTemp|=PHY_BCR_DUPLEX_MODE_;
		}
		if(dwLinkRequest&(LINK_SPEED_100HD|LINK_SPEED_100FD)) {
			wTemp|=PHY_BCR_SPEED_SELECT_;
		}
		Phy_SetRegW(privateData,PHY_BCR,wTemp);
	}
	return 0;
}

static BOOLEAN
Phy_Initialize(PPRIVATE_DATA privateData,
	       DWORD dwPhyAddr,
	       DWORD dwLinkRequest)
{

	BOOLEAN result=FALSE;
	DWORD dwTemp=0;
	WORD wTemp=0;
	DWORD dwLoopCount=0;

	SMSC_TRACE("-->Phy_Initialize");
	SMSC_ASSERT(privateData!=NULL);
	SMSC_ASSERT(privateData->dwLanBase!=0);
	SMSC_ASSERT(dwLinkRequest<=0x7FUL);

	if(dwPhyAddr!=0xFFFFFFFFUL) {
		switch(privateData->dwIdRev&0xFFFF0000) {
		case 0x01170000UL:
		case 0x01150000UL:
			if(privateData->dwIdRev&0x0000FFFF) {
				DWORD dwHwCfg=Lan_GetRegDW(HW_CFG);
				if(dwHwCfg&HW_CFG_EXT_PHY_DET_) {
					/* External phy is requested, supported, and detected */
					/* Attempt to switch */
					/* NOTE: Assuming Rx and Tx are stopped */
					/* because Phy_Initialize is called before  */
					/* Rx_Initialize and Tx_Initialize */
					WORD wPhyId1=0;
					WORD wPhyId2=0;

					/* Disable phy clocks to the mac */
					dwHwCfg&= (~HW_CFG_PHY_CLK_SEL_);
					dwHwCfg|= HW_CFG_PHY_CLK_SEL_CLK_DIS_;
					Lan_SetRegDW(HW_CFG,dwHwCfg);
					udelay(10);/* wait for clocks to acutally stop */

					dwHwCfg|=HW_CFG_EXT_PHY_EN_;
					Lan_SetRegDW(HW_CFG,dwHwCfg);

					dwHwCfg&= (~HW_CFG_PHY_CLK_SEL_);
					dwHwCfg|= HW_CFG_PHY_CLK_SEL_EXT_PHY_;
					Lan_SetRegDW(HW_CFG,dwHwCfg);
					udelay(10);/* wait for clocks to actually start */

					dwHwCfg|=HW_CFG_SMI_SEL_;
					Lan_SetRegDW(HW_CFG,dwHwCfg);

					{
						if(dwPhyAddr<=31) {
							/* only check the phy address specified */
							privateData->dwPhyAddress=dwPhyAddr;
							wPhyId1=Phy_GetRegW(privateData,PHY_ID_1);
							wPhyId2=Phy_GetRegW(privateData,PHY_ID_2);
						} else {
							/* auto detect phy */
							DWORD address=0;
							for(address=0;address<=31;address++) {
								privateData->dwPhyAddress=address;
								if((wPhyId1!=0xFFFFU)||(wPhyId2!=0xFFFFU)) {
									SMSC_TRACE("Detected Phy at address = 0x%02lX = %ld",
										address,address);
									break;
								}
							}
							if(address>=32) {
								SMSC_WARNING("Failed to auto detect external phy");
							}
						}
					}
					if((wPhyId1==0xFFFFU)&&(wPhyId2==0xFFFFU)) {
						SMSC_WARNING("External Phy is not accessable");
						SMSC_WARNING("  using internal phy instead");
						/* revert back to interal phy settings. */

						/* Disable phy clocks to the mac */
						dwHwCfg&= (~HW_CFG_PHY_CLK_SEL_);
						dwHwCfg|= HW_CFG_PHY_CLK_SEL_CLK_DIS_;
						Lan_SetRegDW(HW_CFG,dwHwCfg);
						udelay(10);/* wait for clocks to actually stop */

						dwHwCfg&=(~HW_CFG_EXT_PHY_EN_);
						Lan_SetRegDW(HW_CFG,dwHwCfg);

						dwHwCfg&= (~HW_CFG_PHY_CLK_SEL_);
						dwHwCfg|= HW_CFG_PHY_CLK_SEL_INT_PHY_;
						Lan_SetRegDW(HW_CFG,dwHwCfg);
						udelay(10);/* wait for clocks to actually start */

						dwHwCfg&=(~HW_CFG_SMI_SEL_);
						Lan_SetRegDW(HW_CFG,dwHwCfg);
						goto USE_INTERNAL_PHY;
					} else {
						SMSC_TRACE("Successfully switched to external phy");

					}
				} else {
					SMSC_WARNING("No External Phy Detected");
					SMSC_WARNING("  using internal phy instead");
					goto USE_INTERNAL_PHY;
				}
			} else {
				SMSC_WARNING("External Phy is not supported");
				SMSC_WARNING("  using internal phy instead");
				goto USE_INTERNAL_PHY;
			};break;
		default:
			SMSC_WARNING("External Phy is not supported");
			SMSC_WARNING("  using internal phy instead");
			goto USE_INTERNAL_PHY;
		}
	} else {
USE_INTERNAL_PHY:
		privateData->dwPhyAddress=1;

	}

	dwTemp=Phy_GetRegW(privateData,PHY_ID_2);
	privateData->bPhyRev=((BYTE)(dwTemp&(0x0FUL)));
	privateData->bPhyModel=((BYTE)((dwTemp>>4)&(0x3FUL)));
	privateData->dwPhyId=((dwTemp&(0xFC00UL))<<8);
	dwTemp=Phy_GetRegW(privateData,PHY_ID_1);
	privateData->dwPhyId|=((dwTemp&(0x0000FFFFUL))<<2);

	SMSC_TRACE("dwPhyId==%p,bPhyModel==%p,bPhyRev==%p",
		   privateData->dwPhyId,
		   privateData->bPhyModel,
		   privateData->bPhyRev);

	privateData->dwLinkSpeed=LINK_OFF;
	privateData->dwLinkSettings=LINK_OFF;
	/* reset the PHY */
	Phy_SetRegW(privateData,PHY_BCR,PHY_BCR_RESET_);
	dwLoopCount=100000;
	do {
		udelay(10);
		wTemp=Phy_GetRegW(privateData,PHY_BCR);
		dwLoopCount--;
	} while((dwLoopCount>0) && (wTemp&PHY_BCR_RESET_));

	if(wTemp&PHY_BCR_RESET_) {
		SMSC_WARNING("PHY reset failed to complete.");
		goto DONE;
	}

	if(Phy_SetLink(privateData,dwLinkRequest)<0)
	  return -1;

	result=TRUE;
DONE:
	SMSC_TRACE("<--Phy_Initialize, result=%s",result?"TRUE":"FALSE");
	return result;
}

static void
Rx_ReceiverOn(PPRIVATE_DATA privateData)
{
	if(privateData->dwRxOffCount>0) {
		privateData->dwRxOffCount--;
		if(privateData->dwRxOffCount==0) {
			DWORD dwMacCr=Mac_GetRegDW(privateData,MAC_CR);
			if(dwMacCr&MAC_CR_RXEN_) {
				SMSC_WARNING("Rx_ReceiverOn: Receiver is already on");
			}
			dwMacCr|=MAC_CR_RXEN_;
			Mac_SetRegDW(privateData,MAC_CR,dwMacCr);
		}
	} else {
		SMSC_ASSERT(FALSE);
	}
}

static int
smsc911x_eth_tx_init(void){
	DWORD dwRegVal=0;
	DWORD dwMacCr;
	dwRegVal=Lan_GetRegDW(HW_CFG);
	dwRegVal&=(HW_CFG_TX_FIF_SZ_|0x00000FFFUL);

	dwRegVal|=HW_CFG_SF_;
	Lan_SetRegDW(HW_CFG,dwRegVal);

	dwMacCr=Mac_GetRegDW(privateData,MAC_CR);
	dwMacCr|=(MAC_CR_TXEN_|MAC_CR_HBDIS_);
	Mac_SetRegDW(privateData,MAC_CR,dwMacCr);
	Lan_SetRegDW(TX_CFG,TX_CFG_TX_ON_ | TX_CFG_TXS_DUMP_ | TX_CFG_TXD_DUMP_);

	return 0;
}

int
smsc911x_eth_send(struct net_device *dev, struct net_packet *pkt)
{
	DWORD dwTxCmdA;
	DWORD dwTxCmdB;
	int bufptr=0;
	DWORD dwStatus = 0;
	int dwLoopCount;

	memcpy(EthTxBuffer, pkt->eth, ETH_FRAME_LEN);
	bufptr = ETH_FRAME_LEN;
	memcpy(EthTxBuffer + bufptr, pkt->f2.any, pkt->f2len);
	bufptr += pkt->f2len;
	memcpy(EthTxBuffer + bufptr, pkt->f3.any, pkt->f3len);
	bufptr += pkt->f3len;
	memcpy(EthTxBuffer + bufptr, pkt->f4, pkt->f4len);
	bufptr += pkt->f4len;

	if(bufptr < 60){
		memset(&EthTxBuffer[bufptr], 0, 60-bufptr);
		bufptr=60;
	}

	dwTxCmdA=
	  ((((DWORD)(EthTxBuffer))&0x03UL)<<16) | /* DWORD alignment adjustment */
	  TX_CMD_A_INT_FIRST_SEG_ | TX_CMD_A_INT_LAST_SEG_ |
	  ((DWORD)(bufptr));

	dwTxCmdB=
	  (((DWORD)(bufptr))<<16) |
	  ((DWORD)(bufptr));
	Lan_SetRegDW(TX_DATA_FIFO,dwTxCmdA);
	Lan_SetRegDW(TX_DATA_FIFO,dwTxCmdB);

	Platform_WriteFifo(privateData->dwLanBase,
			   (DWORD *)(((DWORD)(EthTxBuffer))&0xFFFFFFFCUL),
			   (((DWORD)(bufptr))+3+
			    (((DWORD)(EthTxBuffer))&0x03UL))>>2);
	dwLoopCount=bufptr;
	while((dwLoopCount>0) && ((dwStatus=Phy_LBT_GetTxStatus(privateData))==0)) {
		dwLoopCount--;
	}
	if(dwStatus==0) {
		SMSC_WARNING("Failed to Transmit during Packet Test");
		return -1;
	}
	if(dwStatus&0x00008000UL) {
		SMSC_WARNING("Transmit encountered errors during Packet Test");
		return -1;
	}

	return 0;
}

static int
smsc911x_eth_rx_init(void)
{
	DWORD tmp;
	privateData->dwRxOffCount=1;
	Lan_SetRegDW(RX_CFG,0x0);
	tmp = Lan_GetRegDW(FIFO_INT) & 0xffff00ff;
	Lan_SetRegDW(FIFO_INT, tmp | (0x18 << 8));
	Rx_ReceiverOn(privateData);

	return 0;
}

static void
Rx_FastForward(PPRIVATE_DATA privateData,DWORD dwDwordCount)
{
	privateData->RxFastForwardCount++;
	if((dwDwordCount>=4)
		&& (
			(((privateData->dwIdRev&0x0000FFFFUL)==0x00000000UL)
				&& (privateData->dwFpgaRev>=0x36))
			||
			((privateData->dwIdRev&0x0000FFFFUL)!=0UL)
			)
		)
	{
		DWORD dwTimeOut=500;
		Lan_SetRegDW(RX_DP_CTRL,(dwDwordCount|RX_DP_CTRL_FFWD_BUSY_));
		while((dwTimeOut)&&(Lan_GetRegDW(RX_DP_CTRL)&
				RX_DP_CTRL_FFWD_BUSY_))
		{
			udelay(1);
			dwTimeOut--;
		}
		if(dwTimeOut==0) {

			SMSC_WARNING("timed out waiting for RX FFWD to finish, RX_DP_CTRL=0x%08lX",
				Lan_GetRegDW(RX_DP_CTRL));
		}
	} else {
		while(dwDwordCount) {
			DWORD dwTemp=Lan_GetRegDW(RX_DATA_FIFO);
			dwTemp=dwTemp;
			dwDwordCount--;
		}
	}
}

static DWORD
Rx_PopRxStatus(PPRIVATE_DATA privateData)
{
	DWORD result=Lan_GetRegDW(RX_FIFO_INF);
	if((privateData->RxCongested==FALSE)||
		((privateData->RxCongested==TRUE)&&((result&0x00FF0000UL)==0UL)))
	{
		if(result&0x00FF0000UL) {
			DWORD dwIntSts=Lan_GetRegDW(INT_STS);
			if(IS_REV_A(privateData)) {
				if(dwIntSts&INT_STS_RDFL_) {
				  SMSC_WARNING("%s(): INT_STS_RDFL_\n",__FUNCTION__);
					Lan_SetRegDW(INT_STS,INT_STS_RDFL_);
				}
			} else {
				if(dwIntSts&INT_STS_RDFO_) {
				  SMSC_WARNING("%s(): INT_STS_RDFD_\n",__FUNCTION__);
					Lan_SetRegDW(INT_STS,INT_STS_RDFO_);
				}
			}
			if(dwIntSts&INT_STS_RXE_){
			  SMSC_WARNING("%s(): INT_STS_RXE_\n",__FUNCTION__);
			  Lan_SetRegDW(INT_STS, INT_STS_RXE_);
			}
			if(dwIntSts&INT_STS_TXE_){
			  SMSC_WARNING("%s(): INT_STS_TXE_\n",__FUNCTION__);
			  Lan_SetRegDW(INT_STS, INT_STS_TXE_);
			}
			if(dwIntSts&0xff8){
			  SMSC_WARNING("%s(): INT_STS: %p\n", __FUNCTION__, dwIntSts);
			  Lan_SetRegDW(INT_STS, 0xff8);
			}

			if((privateData->RxFlowControlActive==FALSE)||
				((privateData->RxFlowControlActive==TRUE)&&
				 (privateData->RxFlowBurstActive==TRUE)))
			{
				/* Rx status is available, read it */
				result=Lan_GetRegDW(RX_STATUS_FIFO);
				privateData->RxStatusDWReadCount++;
				privateData->LastRxStatus3=
					privateData->LastRxStatus2;
				privateData->LastRxStatus2=
					privateData->LastRxStatus1;
				privateData->LastRxStatus1=result;

				if(privateData->RxOverrun) {
					DWORD dwPacketLength=((result&0x3FFF0000UL)>>16);
					DWORD dwByteCount=((dwPacketLength+2+3)&0xFFFFFFFCUL);
					if((privateData->RxUnloadProgress+dwByteCount)>=
						((privateData->RxMaxDataFifoSize)-16))
					{
						/* This is the packet that crosses the corruption point */
						/* so just ignore it and complete the overrun processing. */
						result=0;
						goto FINISH_OVERRUN_PROCESSING;
					}
					privateData->RxUnloadProgress+=dwByteCount;
					privateData->RxUnloadPacketProgress++;
				}

				privateData->RxFlowCurrentThroughput+=
						((((result&0x3FFF0000UL)>>16)-4UL));
				privateData->RxFlowCurrentPacketCount++;
				privateData->RxFlowCurrentWorkLoad+=
						((((result&0x3FFF0000UL)>>16)-4UL)+privateData->RxFlowParameters.PacketCost);
				if(privateData->RxFlowControlActive) {
					privateData->RxFlowBurstWorkLoad+=
						((((result&0x3FFF0000UL)>>16)-4UL)+privateData->RxFlowParameters.PacketCost);
					if(privateData->RxFlowBurstWorkLoad>=
						privateData->RxFlowBurstMaxWorkLoad)
					{
						privateData->RxFlowBurstActive=FALSE;
					}
				}
			} else {
				result=0;
			}
		}
		else
		{
			if(privateData->RxOverrun) {
				DWORD timeOut;
				DWORD temp;
FINISH_OVERRUN_PROCESSING:
				SMSC_WARNING("rx overrun\n");
				temp=0;
				{
					timeOut=2000;
					while((timeOut>0)&&(!(Lan_GetRegDW(INT_STS)&(INT_STS_RXSTOP_INT_)))) {
						udelay(1);
						timeOut--;
					}
					if(timeOut==0) {
						SMSC_WARNING("Timed out waiting for Rx to Stop\n");
					}
					Lan_SetRegDW(INT_STS,INT_STS_RXSTOP_INT_);
				}


				temp=Lan_GetRegDW(RX_CFG);
				Lan_SetRegDW(RX_CFG,(temp&0x3FFFFFFFUL));
				timeOut=10000000;
				Lan_SetBitsDW(RX_CFG,RX_CFG_RX_DUMP_);
				while((timeOut>0)&&(Lan_GetRegDW(RX_CFG)&(RX_CFG_RX_DUMP_))) {
					udelay(1);
					timeOut--;
				}
				if(timeOut==0) {
					SMSC_WARNING("Timed out waiting for Rx Dump to complete\n");
				}
				Lan_SetRegDW(RX_CFG,temp);

				privateData->RxDumpCount++;
				Lan_SetRegDW(INT_STS,INT_STS_RDFL_);
				Rx_ReceiverOn(privateData);
				privateData->RxOverrun=FALSE;
			}
			result=0;
			privateData->LastReasonForReleasingCPU=1;/* Status FIFO Empty */
		}
	} else {
		/* disable and reenable the INT_EN */
		/* This will allow the deassertion interval to begin */
		DWORD temp=Lan_GetRegDW(INT_EN);
		Lan_SetRegDW(INT_EN,0);
		Lan_SetRegDW(INT_EN,temp);
		result=0;
		privateData->LastReasonForReleasingCPU=2;/* High Congestion */
	}
	return result;
}

static void
Rx_CountErrors(PPRIVATE_DATA privateData,DWORD dwRxStatus)
{
}

int
smsc911x_eth_recv(struct net_device *dev, struct net_packet *pkt, int *timeout)
{
	DWORD dwRxStatus=0;
	PPLATFORM_DATA platformData=NULL;
	int timeout_usec = (*timeout) * 1000;
	privateData->RxCongested=FALSE;
	platformData=&(privateData->PlatformData);

	Lan_SetRegDW(RX_CFG,0x0);
	while(1){
		if(timeout_usec-- <0){
			*timeout = 0;
			SMSC_WARNING("recv timeout\n");
			return -1;
		}

		if((dwRxStatus=Rx_PopRxStatus(privateData))==0){
			udelay(1);
			continue;
		}
		DWORD dwPacketLength=((dwRxStatus&0x3FFF0000UL)>>16);
		Rx_CountErrors(privateData,dwRxStatus);
		if((dwRxStatus&RX_STS_ES_)==0) {
			Platform_ReadFifo(privateData->dwLanBase,
					  ((DWORD *)EthRxBuffer),
					  (dwPacketLength+3)>>2);

			pkt->raw_data_len = dwPacketLength - 4;
			memcpy(pkt->raw_data_addr, EthRxBuffer,
			       pkt->raw_data_len);

			*timeout = timeout_usec / 1000;
			return 0;
		}
		/* if we get here then the packet is to be read */
		/* out of the fifo and discarded */
		dwPacketLength+=(2+3);
		dwPacketLength>>=2;
		Rx_FastForward(privateData,dwPacketLength);
	}

	return -1;
}

static BOOLEAN
Lan_Initialize(PPRIVATE_DATA privateData,
	       DWORD dwIntCfg,
	       DWORD dwTxFifSz,
	       DWORD dwAfcCfg)
{
	BOOLEAN result=FALSE;
	DWORD dwTimeOut=0;
	DWORD dwTemp=0;
	DWORD dwResetCount=3;

	SMSC_TRACE("-->Lan_Initialize");

	/* Reset the LAN911x */
	dwResetCount=1;

	while(dwResetCount>0) {
		Lan_SetRegDW(HW_CFG,HW_CFG_SRST_);
		dwTimeOut=1000000;
		do {
			dwTemp=Lan_GetRegDW(HW_CFG);
			dwTimeOut--;
		} while((dwTimeOut>0)&&(dwTemp&HW_CFG_SRST_));
		if(dwTemp&HW_CFG_SRST_) {
			SMSC_WARNING("  Failed to complete reset.");
			goto DONE;
		}
		dwResetCount--;
	}

	SMSC_ASSERT(dwTxFifSz>=0x00020000UL);
	SMSC_ASSERT(dwTxFifSz<=0x000E0000UL);
	SMSC_ASSERT((dwTxFifSz&(~HW_CFG_TX_FIF_SZ_))==0);

	dwTxFifSz = 0x00040000UL;

	Lan_SetRegDW(HW_CFG,dwTxFifSz);

	privateData->RxMaxDataFifoSize=0;
	switch(dwTxFifSz>>16) {
	case 2:privateData->RxMaxDataFifoSize=13440;break;
	case 3:privateData->RxMaxDataFifoSize=12480;break;
	case 4:privateData->RxMaxDataFifoSize=11520;break;
	case 5:privateData->RxMaxDataFifoSize=10560;break;
	case 6:privateData->RxMaxDataFifoSize=9600;break;
	case 7:privateData->RxMaxDataFifoSize=8640;break;
	case 8:privateData->RxMaxDataFifoSize=7680;break;
	case 9:privateData->RxMaxDataFifoSize=6720;break;
	case 10:privateData->RxMaxDataFifoSize=5760;break;
	case 11:privateData->RxMaxDataFifoSize=4800;break;
	case 12:privateData->RxMaxDataFifoSize=3840;break;
	case 13:privateData->RxMaxDataFifoSize=2880;break;
	case 14:privateData->RxMaxDataFifoSize=1920;break;
	default:SMSC_ASSERT(FALSE);break;
	}

	if(dwAfcCfg==0xFFFFFFFF) {
		switch(dwTxFifSz) {
		/* AFC_HI is about ((Rx Data Fifo Size)*2/3)/64 */
		/* AFC_LO is AFC_HI/2 */
		/* BACK_DUR is about 5uS*(AFC_LO) rounded down */
		case 0x00020000UL:/* 13440 Rx Data Fifo Size */
			dwAfcCfg=0x008C46AF;break;
		case 0x00030000UL:/* 12480 Rx Data Fifo Size */
			dwAfcCfg=0x0082419F;break;
		case 0x00040000UL:/* 11520 Rx Data Fifo Size */
			dwAfcCfg=0x00783C9F;break;
		case 0x00050000UL:/* 10560 Rx Data Fifo Size */
			dwAfcCfg=0x006E374F;break;
		case 0x00060000UL:/* 9600 Rx Data Fifo Size */
			dwAfcCfg=0x0064328F;break;
		case 0x00070000UL:/* 8640 Rx Data Fifo Size */
			dwAfcCfg=0x005A2D7F;break;
		case 0x00080000UL:/* 7680 Rx Data Fifo Size */
			dwAfcCfg=0x0050287F;break;
		case 0x00090000UL:/* 6720 Rx Data Fifo Size */
			dwAfcCfg=0x0046236F;break;
		case 0x000A0000UL:/* 5760 Rx Data Fifo Size */
			dwAfcCfg=0x003C1E6F;break;
		case 0x000B0000UL:/* 4800 Rx Data Fifo Size */
			dwAfcCfg=0x0032195F;break;

		/* AFC_HI is ~1520 bytes less than RX Data Fifo Size */
		/* AFC_LO is AFC_HI/2 */
		/* BACK_DUR is about 5uS*(AFC_LO) rounded down */
		case 0x000C0000UL:/* 3840 Rx Data Fifo Size */
			dwAfcCfg=0x0024124F;break;
		case 0x000D0000UL:/* 2880 Rx Data Fifo Size */
			dwAfcCfg=0x0015073F;break;
		case 0x000E0000UL:/* 1920 Rx Data Fifo Size */
			dwAfcCfg=0x0006032F;break;
		default:SMSC_ASSERT(FALSE);break;
		}
	}
	Lan_SetRegDW(AFC_CFG,(dwAfcCfg&0xFFFFFFF0UL));

	/* make sure EEPROM has finished loading before setting GPIO_CFG */
	dwTimeOut=1000;
	while((dwTimeOut>0)&&(Lan_GetRegDW(E2P_CMD)&E2P_CMD_EPC_BUSY_)) {
		dwTimeOut--;
	}
	if(dwTimeOut==0) {
		SMSC_WARNING("Lan_Initialize: Timed out waiting for EEPROM busy bit to clear\n");
	}

	Lan_SetRegDW(GPIO_CFG,0x70000000UL);

	/* initialize interrupts */
	Lan_SetRegDW(INT_EN,0);
	Lan_SetRegDW(INT_STS,0xFFFFFFFFUL);

	result=TRUE;

DONE:
	SMSC_TRACE("<--Lan_Initialize");
	return result;
}

static int
smsc911x_open(PPRIVATE_DATA privateData)
{
	if(!Lan_Initialize(privateData,0,0x00050000,0xFFFFFFFF)) {
		SMSC_WARNING("Failed Lan_Initialize");
		return -ENODEV;
	}

	SMSC_TRACE("<--Smsc911x_open");
	return 0;
}

void
smsc911x_enable_phy_module(struct net_device *dev)
{
}

void
smsc911x_disable_phy_module(struct net_device *dev)
{
}

int
smsc911x_get_mac(unsigned char *mac)
{
	struct net_device *dev = pinfo->net_dev;
	int i;
	unsigned long mac_hi, mac_low;

	if (!dev)
		return -1;

	privateData->dwLanBase=dev->base_addr;
	mac_hi  = Mac_GetRegDW(privateData,ADDRH);
	mac_low = Mac_GetRegDW(privateData,ADDRL);

	if((mac_hi == 0x0000ffff) && (mac_low == 0xffffffff)){
		SMSC_WARNING("Error: read MAC addr."
			     "using default 00:01:02:03:04:05.\n");
		for(i=0;i<6;i++)
			mac[i] = i;
	}else{
		mac[0] = (mac_low >>  0) & 0xff;
		mac[1] = (mac_low >>  8) & 0xff;
		mac[2] = (mac_low >> 16) & 0xff;
		mac[3] = (mac_low >> 24) & 0xff;
		mac[4] = (mac_hi  >>  0) & 0xff;
		mac[5] = (mac_hi  >>  8) & 0xff;
	}
	return 0;
}

int
smsc911x_eth_init(struct net_device *dev)
{
	DWORD dwLanBase=0UL;
	DWORD dwIdRev=0UL;
	DWORD dwFpgaRev=0UL;

	int result=-ENODEV;
	int ret;

	memzero(&_privateData, sizeof(_platformData));
	memzero(&_platformData, sizeof(_platformData));

	dwLanBase=Platform_Initialize(platformData, 0, 16);
	if (dwLanBase == -1)
		return -H_EIO;

	privateData->dwLanBase=dwLanBase;
	dwIdRev=Lan_GetRegDW(ID_REV);
	if(HIWORD(dwIdRev)==LOWORD(dwIdRev)) {
		/* this may mean the chip is set for 32 bit  */
		/* while the bus is reading as 16 bit */
		SMSC_WARNING("  LAN911x NOT Identified, dwIdRev==%p",dwIdRev);
		result=-ENODEV;
		return -1;
	}

	SMSC_TRACE("  LAN911x identified, dwIdRev==%p", dwIdRev);
	dwFpgaRev=Lan_GetRegDW(FPGA_REV);
	SMSC_TRACE("  FPGA_REV == %p",dwFpgaRev);

	ret = Phy_Initialize(privateData, 0xffffffff,0x7f );
	ret = smsc911x_open(privateData);

	ret = Phy_CheckLink((unsigned long)privateData);
	if(ret < 0){
		return -1;
	}

	dev->enable_phy = smsc911x_enable_phy_module;
	dev->disable_phy = smsc911x_disable_phy_module;
	dev->send = smsc911x_eth_send;
	dev->recv = smsc911x_eth_recv;
	get_mac_address(dev->eth_mac);

	smsc911x_eth_rx_init();
	smsc911x_eth_tx_init();

	Lan_SetRegDW(FIFO_INT, 0x48ff2020);
	/* Interrupt Status Clear */
	Lan_SetRegDW(INT_STS, 0xffffffff);

	return 0;
}

struct net_device smsc911x = {
	.init		= smsc911x_eth_init,
};
