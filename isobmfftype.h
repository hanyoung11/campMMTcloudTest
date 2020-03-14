#include <stdint.h>
#include <string.h>

/*
//attributes
tesc
fgsc
cgsc
spsc
resc
vwsc
cdec
scsz
mpsz
mtyp
mela
bitr
frar
nvws


//brand name
isom
avc1
sdtp
sbgp
sgpd
iso2
iso3
iso4
iso5
iso6
iso7
iso8
iso9

*/
	//box type uint
#define BTYPE_UUID 0x75756964 //uuid
#define BTYPE_FTYP 0x66747970 //ftyp
#define BTYPE_MDAT 0x6d646174 //mdat
#define BTYPE_MOOV 0x6d6f6f76 //moov
#define BTYPE_MVHD 0x6d766864 //mvhd
#define BTYPE_TRAK 0x7472616b //trak
#define BTYPE_TKHD 0x746b6864 //tkhd
#define BTYPE_TREF 0x74726566 //tref
#define BTYPE_TRGR 0x74726772 //trgr
#define BTYPE_MDIA 0x6d646961 //mdia
#define BTYPE_MDHD 0x6d646864 //mdhd
#define BTYPE_HDLR 0x68646c72 //hdlr
#define BTYPE_MINF 0x6d696e66 //minf
#define BTYPE_ELNG 0x656c6e67 //elng
#define BTYPE_STBL 0x7374626c //stbl
#define BTYPE_STDP 0x73746470 //stdp
#define BTYPE_STTS 0x73747473 //stts
#define BTYPE_CTTS 0x63747473 //ctts
#define BTYPE_CSLG 0x63736c67 //cslg
#define BTYPE_STSS 0x73747373 //stss
#define BTYPE_STSH 0x73747368 //stsh
#define BTYPE_EDTS 0x65647473 //edts
#define BTYPE_ELST 0x656c7374 //elst
#define BTYPE_BTRT 0x62747274 //btrt
#define BTYPE_DINF 0x64696e66 //dinf
#define BTYPE_STSZ 0x7374737a //stsz
#define BTYPE_STZ2 0x73747a32 //stz2
#define BTYPE_STSC 0x73747363 //stsc
#define BTYPE_STCO 0x7374636f //stco
#define BTYPE_CO64 0x636f3634 //co64
#define BTYPE_PADB 0x70616462 //padb
#define BTYPE_SUBS 0x73756273 //subs
#define BTYPE_SAIZ 0x7361697a //saiz
#define BTYPE_SAIO 0x7361696f //saio
#define BTYPE_MVEX 0x6d766578 //mvex
#define BTYPE_MEHD 0x6d656864 //mehd
#define BTYPE_TREX 0x74726578 //trex
#define BTYPE_MOOF 0x6d6f6f66 //moof
#define BTYPE_MFHD 0x6d666864 //mfhd
#define BTYPE_TRAF 0x74726166 //traf
#define BTYPE_TFHD 0x74666864 //tfhd
#define BTYPE_TRUN 0x7472756e //trun
#define BTYPE_MFRA 0x6d667261 //mfra
#define BTYPE_TFRA 0x74667261 //tfra
#define BTYPE_MFRO 0x6d66726f //mfro
#define BTYPE_TFDT 0x74666474 //tfdt
#define BTYPE_LEVA 0x6c657661 //leva
#define BTYPE_TREP 0x74726570 //trep
#define BTYPE_ASSP 0x61737370 //assp
#define BTYPE_SBGP 0x73626770 //sbgp
#define BTYPE_SGPD 0x73677064 //sgpd
#define BTYPE_UDTA 0x75647461 //udta
#define BTYPE_CPRT 0x63707274 //cprt
#define BTYPE_TSEL 0x7473656c //tsel
#define BTYPE_KIND 0x6b696e64 //kind
#define BTYPE_META 0x6d657461 //meta
#define BTYPE_XML  0x786d6c20 //xml
#define BTYPE_BXML 0x62786d6c //bxml
#define BTYPE_ILOC 0x696c6f63 //iloc
#define BTYPE_PITM 0x7069746d //pitm
#define BTYPE_IPRO 0x6970726f //ipro
#define BTYPE_IINF 0x69696e66 //iinf
#define BTYPE_MECO 0x6d65636f //meco
#define BTYPE_MERE 0x6d657265 //mere
#define BTYPE_IDAT 0x69646174 //idat
#define BTYPE_IREF 0x69726566 //iref
#define BTYPE_FIIN 0x6669696e //fiin
#define BTYPE_FPAR 0x66706172 //fpar
#define BTYPE_FECR 0x66656372 //fecr
#define BTYPE_SEGR 0x73656772 //segr
#define BTYPE_GITN 0x6769746e //gitn
#define BTYPE_FIRE 0x66697265 //fire
#define BTYPE_STRK 0x7374726b //strk
#define BTYPE_STRI 0x73747269 //stri
#define BTYPE_STRD 0x73747264 //strd
#define BTYPE_STSG 0x73747367 //stsg
#define BTYPE_STVI 0x73747669 //stvi
#define BTYPE_STYP 0x73747970 //styp
#define BTYPE_SIDX 0x73696478 //sidx
#define BTYPE_SSIX 0x73736978 //ssix
#define BTYPE_PRFT 0x70726674 //prft
#define BTYPE_SRPP 0x73727070 //srpp
#define BTYPE_FECI 0x66656369 //feci
#define BTYPE_FREE 0x66726565 //free
#define BTYPE_SKIP 0x736b6970 //skip
#define BTYPE_PDIN 0x7064696e //pdin
#define BTYPE_NMHD 0x6e6d6864 //nmhd
#define BTYPE_STSD 0x73747364 //stsd
#define BTYPE_SDTP 0x73647470 //sdtp
#define BTYPE_DREF 0x64726566 //dref
#define BTYPE_URL  0x75726c20 //url
#define BTYPE_URN  0x75726e20 //urn
#define BTYPE_SINF 0x73696e66 //sinf
#define BTYPE_FRMA 0x66726d61 //frma
#define BTYPE_SCHM 0x7363686d //schm
#define BTYPE_SCHI 0x73636869 //schi
#define BTYPE_RINF 0x72696e66 //rinf
#define BTYPE_CINF 0x63696e66 //cinf
#define BTYPE_VMHD 0x766d6864 //vmhd
#define BTYPE_SMHD 0x736d6864 //smhd
#define BTYPE_CHNL 0x63686e6c //chnl
#define BTYPE_DMIX 0x646d6978 //dmix
#define BTYPE_LUDT 0x6c756474 //ludt
#define BTYPE_HMHD 0x686d6864 //hmhd
#define BTYPE_INFE 0x696e6665 //infe
#define BTYPE_SRAT 0x73726174 //srat
#define BTYPE_TXTC 0x74787443 //txtC
#define BTYPE_URI  0x75726920 //uri
#define BTYPE_URII 0x75726949 //uriI
#define BTYPE_STHD 0x73746864 //sthd
//MMT Box (ISO_IEC_23008-1)
#define BTYPE_MMPU 0x6d6d7075 //mmpu
#define BTYPE_MULI 0x6d756c69//muli

	//box type char
#define CTYPE_FTYP "ftyp"
#define CTYPE_MDAT "mdat"
#define CTYPE_MOOV "moov"
#define CTYPE_MVHD "mvhd"
#define CTYPE_TRAK "trak"
#define CTYPE_TKHD "tkhd"
#define CTYPE_TREF "tref"
#define CTYPE_TRGR "trgr"
#define CTYPE_MDIA "mdia"
#define CTYPE_MDHD "mdhd"
#define CTYPE_HDLR "hdlr"
#define CTYPE_MINF "minf"
#define CTYPE_ELNG "elng"
#define CTYPE_STBL "stbl"
#define CTYPE_STDP "stdp"
#define CTYPE_STTS "stts"
#define CTYPE_CTTS "ctts"
#define CTYPE_CSLG "cslg"
#define CTYPE_STSS "stss"
#define CTYPE_STSH "stsh"
#define CTYPE_EDTS "edts"
#define CTYPE_ELST "elst"
#define CTYPE_BTRT "btrt"
#define CTYPE_DINF "dinf"
#define CTYPE_STSZ "stsz"
#define CTYPE_STZ2 "stz2"
#define CTYPE_STSC "stsc"
#define CTYPE_STCO "stco"
#define CTYPE_CO64 "co64"
#define CTYPE_PADB "padb"
#define CTYPE_SUBS "subs"
#define CTYPE_SAIZ "saiz"
#define CTYPE_SAIO "saio"
#define CTYPE_MVEX "mvex"
#define CTYPE_MEHD "mehd"
#define CTYPE_TREX "trex"
#define CTYPE_MOOF "moof"
#define CTYPE_MFHD "mfhd"
#define CTYPE_TRAF "traf"
#define CTYPE_TFHD "tfhd"
#define CTYPE_TRUN "trun"
#define CTYPE_MFRA "mfra"
#define CTYPE_TFRA "tfra"
#define CTYPE_MFRO "mfro"
#define CTYPE_TFDT "tfdt"
#define CTYPE_LEVA "leva"
#define CTYPE_TREP "trep"
#define CTYPE_ASSP "assp"
#define CTYPE_SBGP "sbgp"
#define CTYPE_SGPD "sgpd"
#define CTYPE_UDTA "udta"
#define CTYPE_CPRT "cprt"
#define CTYPE_TSEL "tsel"
#define CTYPE_KIND "kind"
#define CTYPE_META "meta"
#define CTYPE_XML  "xml "
#define CTYPE_BXML "bxml"
#define CTYPE_ILOC "iloc"
#define CTYPE_PITM "pitm"
#define CTYPE_IPRO "ipro"
#define CTYPE_IINF "iinf"
#define CTYPE_MECO "meco"
#define CTYPE_MERE "mere"
#define CTYPE_IDAT "idat"
#define CTYPE_IREF "iref"
#define CTYPE_FIIN "fiin"
#define CTYPE_FPAR "fpar"
#define CTYPE_FECR "fecr"
#define CTYPE_SEGR "segr"
#define CTYPE_GITN "gitn"
#define CTYPE_FIRE "fire"
#define CTYPE_STRK "strk"
#define CTYPE_STRI "stri"
#define CTYPE_STRD "strd"
#define CTYPE_STSG "stsg"
#define CTYPE_STVI "stvi"
#define CTYPE_STYP "styp"
#define CTYPE_SIDX "sidx"
#define CTYPE_SSIX "ssix"
#define CTYPE_PRFT "prft"
#define CTYPE_SRPP "srpp"
#define CTYPE_FECI "feci"
#define CTYPE_FREE "free"
#define CTYPE_SKIP "skip"
#define CTYPE_PDIN "pdin"
#define CTYPE_NMHD "nmhd"
#define CTYPE_STSD "stsd"
#define CTYPE_SDTP "sdtp"
#define CTYPE_DREF "dref"
#define CTYPE_URL  "url "
#define CTYPE_URN  "urn "
#define CTYPE_SINF "sinf"
#define CTYPE_FRMA "frma"
#define CTYPE_SCHM "schm"
#define CTYPE_SCHI "schi"
#define CTYPE_RINF "rinf"
#define CTYPE_CINF "cinf"
#define CTYPE_VMHD "vmhd"
#define CTYPE_SMHD "smhd"
#define CTYPE_CHNL "chnl"
#define CTYPE_DMIX "dmix"
#define CTYPE_LUDT "ludt"
#define CTYPE_HMHD "hmhd"
#define CTYPE_INFE "infe"
#define CTYPE_SRAT "srat"
#define CTYPE_TXTC "txtC"
#define CTYPE_URI  "uri "
#define CTYPE_URII "uriI"
#define CTYPE_STHD "sthd"
//MMT Box (ISO_IEC_23008-1)
#define CTYPE_MMPU "mmpu"
#define CTYPE_MULI "muli"


static uint32_t cui_conversion(const char* ctype) {
	if (strncmp(ctype, CTYPE_FTYP, 4) == 0)return BTYPE_FTYP;
	else if (strncmp(ctype, CTYPE_MDAT, 4) == 0)return BTYPE_MDAT;
	else if (strncmp(ctype, CTYPE_MOOV, 4) == 0)return BTYPE_MOOV;
	else if (strncmp(ctype, CTYPE_MVHD, 4) == 0)return BTYPE_MVHD;
	else if (strncmp(ctype, CTYPE_TRAK, 4) == 0)return BTYPE_TRAK;
	else if (strncmp(ctype, CTYPE_TKHD, 4) == 0)return BTYPE_TKHD;
	else if (strncmp(ctype, CTYPE_TREF, 4) == 0)return BTYPE_TREF;
	else if (strncmp(ctype, CTYPE_TRGR, 4) == 0)return BTYPE_TRGR;
	else if (strncmp(ctype, CTYPE_MDIA, 4) == 0)return BTYPE_MDIA;
	else if (strncmp(ctype, CTYPE_MDHD, 4) == 0)return BTYPE_MDHD;
	else if (strncmp(ctype, CTYPE_HDLR, 4) == 0)return BTYPE_HDLR;
	else if (strncmp(ctype, CTYPE_MINF, 4) == 0)return BTYPE_MINF;
	else if (strncmp(ctype, CTYPE_ELNG, 4) == 0)return BTYPE_ELNG;
	else if (strncmp(ctype, CTYPE_STBL, 4) == 0)return BTYPE_STBL;
	else if (strncmp(ctype, CTYPE_STDP, 4) == 0)return BTYPE_STDP;
	else if (strncmp(ctype, CTYPE_STTS, 4) == 0)return BTYPE_STTS;
	else if (strncmp(ctype, CTYPE_CTTS, 4) == 0)return BTYPE_CTTS;
	else if (strncmp(ctype, CTYPE_CSLG, 4) == 0)return BTYPE_CSLG;
	else if (strncmp(ctype, CTYPE_STSS, 4) == 0)return BTYPE_STSS;
	else if (strncmp(ctype, CTYPE_STSH, 4) == 0)return BTYPE_STSH;
	else if (strncmp(ctype, CTYPE_EDTS, 4) == 0)return BTYPE_EDTS;
	else if (strncmp(ctype, CTYPE_ELST, 4) == 0)return BTYPE_ELST;
	else if (strncmp(ctype, CTYPE_BTRT, 4) == 0)return BTYPE_BTRT;
	else if (strncmp(ctype, CTYPE_DINF, 4) == 0)return BTYPE_DINF;
	else if (strncmp(ctype, CTYPE_STSZ, 4) == 0)return BTYPE_STSZ;
	else if (strncmp(ctype, CTYPE_STZ2, 4) == 0)return BTYPE_STZ2;
	else if (strncmp(ctype, CTYPE_STSC, 4) == 0)return BTYPE_STSC;
	else if (strncmp(ctype, CTYPE_STCO, 4) == 0)return BTYPE_STCO;
	else if (strncmp(ctype, CTYPE_CO64, 4) == 0)return BTYPE_CO64;
	else if (strncmp(ctype, CTYPE_PADB, 4) == 0)return BTYPE_PADB;
	else if (strncmp(ctype, CTYPE_SUBS, 4) == 0)return BTYPE_SUBS;
	else if (strncmp(ctype, CTYPE_SAIZ, 4) == 0)return BTYPE_SAIZ;
	else if (strncmp(ctype, CTYPE_SAIO, 4) == 0)return BTYPE_SAIO;
	else if (strncmp(ctype, CTYPE_MVEX, 4) == 0)return BTYPE_MVEX;
	else if (strncmp(ctype, CTYPE_MEHD, 4) == 0)return BTYPE_MEHD;
	else if (strncmp(ctype, CTYPE_TREX, 4) == 0)return BTYPE_TREX;
	else if (strncmp(ctype, CTYPE_MOOF, 4) == 0)return BTYPE_MOOF;
	else if (strncmp(ctype, CTYPE_MFHD, 4) == 0)return BTYPE_MFHD;
	else if (strncmp(ctype, CTYPE_TRAF, 4) == 0)return BTYPE_TRAF;
	else if (strncmp(ctype, CTYPE_TFHD, 4) == 0)return BTYPE_TFHD;
	else if (strncmp(ctype, CTYPE_TRUN, 4) == 0)return BTYPE_TRUN;
	else if (strncmp(ctype, CTYPE_MFRA, 4) == 0)return BTYPE_MFRA;
	else if (strncmp(ctype, CTYPE_TFRA, 4) == 0)return BTYPE_TFRA;
	else if (strncmp(ctype, CTYPE_MFRO, 4) == 0)return BTYPE_MFRO;
	else if (strncmp(ctype, CTYPE_TFDT, 4) == 0)return BTYPE_TFDT;
	else if (strncmp(ctype, CTYPE_LEVA, 4) == 0)return BTYPE_LEVA;
	else if (strncmp(ctype, CTYPE_TREP, 4) == 0)return BTYPE_TREP;
	else if (strncmp(ctype, CTYPE_ASSP, 4) == 0)return BTYPE_ASSP;
	else if (strncmp(ctype, CTYPE_SBGP, 4) == 0)return BTYPE_SBGP;
	else if (strncmp(ctype, CTYPE_SGPD, 4) == 0)return BTYPE_SGPD;
	else if (strncmp(ctype, CTYPE_UDTA, 4) == 0)return BTYPE_UDTA;
	else if (strncmp(ctype, CTYPE_CPRT, 4) == 0)return BTYPE_CPRT;
	else if (strncmp(ctype, CTYPE_TSEL, 4) == 0)return BTYPE_TSEL;
	else if (strncmp(ctype, CTYPE_KIND, 4) == 0)return BTYPE_KIND;
	else if (strncmp(ctype, CTYPE_META, 4) == 0)return BTYPE_META;
	else if (strncmp(ctype, CTYPE_XML , 4) == 0)return BTYPE_XML ;
	else if (strncmp(ctype, CTYPE_BXML, 4) == 0)return BTYPE_BXML;
	else if (strncmp(ctype, CTYPE_ILOC, 4) == 0)return BTYPE_ILOC;
	else if (strncmp(ctype, CTYPE_PITM, 4) == 0)return BTYPE_PITM;
	else if (strncmp(ctype, CTYPE_IPRO, 4) == 0)return BTYPE_IPRO;
	else if (strncmp(ctype, CTYPE_IINF, 4) == 0)return BTYPE_IINF;
	else if (strncmp(ctype, CTYPE_MECO, 4) == 0)return BTYPE_MECO;
	else if (strncmp(ctype, CTYPE_MERE, 4) == 0)return BTYPE_MERE;
	else if (strncmp(ctype, CTYPE_IDAT, 4) == 0)return BTYPE_IDAT;
	else if (strncmp(ctype, CTYPE_IREF, 4) == 0)return BTYPE_IREF;
	else if (strncmp(ctype, CTYPE_FIIN, 4) == 0)return BTYPE_FIIN;
	else if (strncmp(ctype, CTYPE_FPAR, 4) == 0)return BTYPE_FPAR;
	else if (strncmp(ctype, CTYPE_FECR, 4) == 0)return BTYPE_FECR;
	else if (strncmp(ctype, CTYPE_SEGR, 4) == 0)return BTYPE_SEGR;
	else if (strncmp(ctype, CTYPE_GITN, 4) == 0)return BTYPE_GITN;
	else if (strncmp(ctype, CTYPE_FIRE, 4) == 0)return BTYPE_FIRE;
	else if (strncmp(ctype, CTYPE_STRK, 4) == 0)return BTYPE_STRK;
	else if (strncmp(ctype, CTYPE_STRI, 4) == 0)return BTYPE_STRI;
	else if (strncmp(ctype, CTYPE_STRD, 4) == 0)return BTYPE_STRD;
	else if (strncmp(ctype, CTYPE_STSG, 4) == 0)return BTYPE_STSG;
	else if (strncmp(ctype, CTYPE_STVI, 4) == 0)return BTYPE_STVI;
	else if (strncmp(ctype, CTYPE_STYP, 4) == 0)return BTYPE_STYP;
	else if (strncmp(ctype, CTYPE_SIDX, 4) == 0)return BTYPE_SIDX;
	else if (strncmp(ctype, CTYPE_SSIX, 4) == 0)return BTYPE_SSIX;
	else if (strncmp(ctype, CTYPE_PRFT, 4) == 0)return BTYPE_PRFT;
	else if (strncmp(ctype, CTYPE_SRPP, 4) == 0)return BTYPE_SRPP;
	else if (strncmp(ctype, CTYPE_FECI, 4) == 0)return BTYPE_FECI;
	else if (strncmp(ctype, CTYPE_FREE, 4) == 0)return BTYPE_FREE;
	else if (strncmp(ctype, CTYPE_SKIP, 4) == 0)return BTYPE_SKIP;
	else if (strncmp(ctype, CTYPE_PDIN, 4) == 0)return BTYPE_PDIN;
	else if (strncmp(ctype, CTYPE_NMHD, 4) == 0)return BTYPE_NMHD;
	else if (strncmp(ctype, CTYPE_STSD, 4) == 0)return BTYPE_STSD;
	else if (strncmp(ctype, CTYPE_SDTP, 4) == 0)return BTYPE_SDTP;
	else if (strncmp(ctype, CTYPE_DREF, 4) == 0)return BTYPE_DREF;
	else if (strncmp(ctype, CTYPE_URL , 4) == 0)return BTYPE_URL ;
	else if (strncmp(ctype, CTYPE_URN , 4) == 0)return BTYPE_URN ;
	else if (strncmp(ctype, CTYPE_SINF, 4) == 0)return BTYPE_SINF;
	else if (strncmp(ctype, CTYPE_FRMA, 4) == 0)return BTYPE_FRMA;
	else if (strncmp(ctype, CTYPE_SCHM, 4) == 0)return BTYPE_SCHM;
	else if (strncmp(ctype, CTYPE_SCHI, 4) == 0)return BTYPE_SCHI;
	else if (strncmp(ctype, CTYPE_RINF, 4) == 0)return BTYPE_RINF;
	else if (strncmp(ctype, CTYPE_CINF, 4) == 0)return BTYPE_CINF;
	else if (strncmp(ctype, CTYPE_VMHD, 4) == 0)return BTYPE_VMHD;
	else if (strncmp(ctype, CTYPE_SMHD, 4) == 0)return BTYPE_SMHD;
	else if (strncmp(ctype, CTYPE_CHNL, 4) == 0)return BTYPE_CHNL;
	else if (strncmp(ctype, CTYPE_DMIX, 4) == 0)return BTYPE_DMIX;
	else if (strncmp(ctype, CTYPE_LUDT, 4) == 0)return BTYPE_LUDT;
	else if (strncmp(ctype, CTYPE_HMHD, 4) == 0)return BTYPE_HMHD;
	else if (strncmp(ctype, CTYPE_INFE, 4) == 0)return BTYPE_INFE;
	else if (strncmp(ctype, CTYPE_SRAT, 4) == 0)return BTYPE_SRAT;
	else if (strncmp(ctype, CTYPE_TXTC, 4) == 0)return BTYPE_TXTC;
	else if (strncmp(ctype, CTYPE_URI, 4) == 0)return BTYPE_URI;
	else if (strncmp(ctype, CTYPE_URII, 4) == 0)return BTYPE_URII;
	else if (strncmp(ctype, CTYPE_STHD, 4) == 0)return BTYPE_STHD;
	else if (strncmp(ctype, CTYPE_MMPU, 4) == 0)return BTYPE_MMPU;
	else if (strncmp(ctype, CTYPE_MULI, 4) == 0)return BTYPE_MULI;
}

static const char* uic_conversion(uint32_t btype) {
	switch (btype)
	{
	case BTYPE_FTYP: return CTYPE_FTYP;
	case BTYPE_MDAT: return CTYPE_MDAT;
	case BTYPE_MOOV: return CTYPE_MOOV;
	case BTYPE_MVHD: return CTYPE_MVHD;
	case BTYPE_TRAK: return CTYPE_TRAK;
	case BTYPE_TKHD: return CTYPE_TKHD;
	case BTYPE_TREF: return CTYPE_TREF;
	case BTYPE_TRGR: return CTYPE_TRGR;
	case BTYPE_MDIA: return CTYPE_MDIA;
	case BTYPE_MDHD: return CTYPE_MDHD;
	case BTYPE_HDLR: return CTYPE_HDLR;
	case BTYPE_MINF: return CTYPE_MINF;
	case BTYPE_ELNG: return CTYPE_ELNG;
	case BTYPE_STBL: return CTYPE_STBL;
	case BTYPE_STDP: return CTYPE_STDP;
	case BTYPE_STTS: return CTYPE_STTS;
	case BTYPE_CTTS: return CTYPE_CTTS;
	case BTYPE_CSLG: return CTYPE_CSLG;
	case BTYPE_STSS: return CTYPE_STSS;
	case BTYPE_STSH: return CTYPE_STSH;
	case BTYPE_EDTS: return CTYPE_EDTS;
	case BTYPE_ELST: return CTYPE_ELST;
	case BTYPE_BTRT: return CTYPE_BTRT;
	case BTYPE_DINF: return CTYPE_DINF;
	case BTYPE_STSZ: return CTYPE_STSZ;
	case BTYPE_STZ2: return CTYPE_STZ2;
	case BTYPE_STSC: return CTYPE_STSC;
	case BTYPE_STCO: return CTYPE_STCO;
	case BTYPE_CO64: return CTYPE_CO64;
	case BTYPE_PADB: return CTYPE_PADB;
	case BTYPE_SUBS: return CTYPE_SUBS;
	case BTYPE_SAIZ: return CTYPE_SAIZ;
	case BTYPE_SAIO: return CTYPE_SAIO;
	case BTYPE_MVEX: return CTYPE_MVEX;
	case BTYPE_MEHD: return CTYPE_MEHD;
	case BTYPE_TREX: return CTYPE_TREX;
	case BTYPE_MOOF: return CTYPE_MOOF;
	case BTYPE_MFHD: return CTYPE_MFHD;
	case BTYPE_TRAF: return CTYPE_TRAF;
	case BTYPE_TFHD: return CTYPE_TFHD;
	case BTYPE_TRUN: return CTYPE_TRUN;
	case BTYPE_MFRA: return CTYPE_MFRA;
	case BTYPE_TFRA: return CTYPE_TFRA;
	case BTYPE_MFRO: return CTYPE_MFRO;
	case BTYPE_TFDT: return CTYPE_TFDT;
	case BTYPE_LEVA: return CTYPE_LEVA;
	case BTYPE_TREP: return CTYPE_TREP;
	case BTYPE_ASSP: return CTYPE_ASSP;
	case BTYPE_SBGP: return CTYPE_SBGP;
	case BTYPE_SGPD: return CTYPE_SGPD;
	case BTYPE_UDTA: return CTYPE_UDTA;
	case BTYPE_CPRT: return CTYPE_CPRT;
	case BTYPE_TSEL: return CTYPE_TSEL;
	case BTYPE_KIND: return CTYPE_KIND;
	case BTYPE_META: return CTYPE_META;
	case BTYPE_XML : return CTYPE_XML ;
	case BTYPE_BXML: return CTYPE_BXML;
	case BTYPE_ILOC: return CTYPE_ILOC;
	case BTYPE_PITM: return CTYPE_PITM;
	case BTYPE_IPRO: return CTYPE_IPRO;
	case BTYPE_IINF: return CTYPE_IINF;
	case BTYPE_MECO: return CTYPE_MECO;
	case BTYPE_MERE: return CTYPE_MERE;
	case BTYPE_IDAT: return CTYPE_IDAT;
	case BTYPE_IREF: return CTYPE_IREF;
	case BTYPE_FIIN: return CTYPE_FIIN;
	case BTYPE_FPAR: return CTYPE_FPAR;
	case BTYPE_FECR: return CTYPE_FECR;
	case BTYPE_SEGR: return CTYPE_SEGR;
	case BTYPE_GITN: return CTYPE_GITN;
	case BTYPE_FIRE: return CTYPE_FIRE;
	case BTYPE_STRK: return CTYPE_STRK;
	case BTYPE_STRI: return CTYPE_STRI;
	case BTYPE_STRD: return CTYPE_STRD;
	case BTYPE_STSG: return CTYPE_STSG;
	case BTYPE_STVI: return CTYPE_STVI;
	case BTYPE_STYP: return CTYPE_STYP;
	case BTYPE_SIDX: return CTYPE_SIDX;
	case BTYPE_SSIX: return CTYPE_SSIX;
	case BTYPE_PRFT: return CTYPE_PRFT;
	case BTYPE_SRPP: return CTYPE_SRPP;
	case BTYPE_FECI: return CTYPE_FECI;
	case BTYPE_FREE: return CTYPE_FREE;
	case BTYPE_SKIP: return CTYPE_SKIP;
	case BTYPE_PDIN: return CTYPE_PDIN;
	case BTYPE_NMHD: return CTYPE_NMHD;
	case BTYPE_STSD: return CTYPE_STSD;
	case BTYPE_SDTP: return CTYPE_SDTP;
	case BTYPE_DREF: return CTYPE_DREF;
	case BTYPE_URL : return CTYPE_URL ;
	case BTYPE_URN : return CTYPE_URN ;
	case BTYPE_SINF: return CTYPE_SINF;
	case BTYPE_FRMA: return CTYPE_FRMA;
	case BTYPE_SCHM: return CTYPE_SCHM;
	case BTYPE_SCHI: return CTYPE_SCHI;
	case BTYPE_RINF: return CTYPE_RINF;
	case BTYPE_CINF: return CTYPE_CINF;
	case BTYPE_VMHD: return CTYPE_VMHD;
	case BTYPE_SMHD: return CTYPE_SMHD;
	case BTYPE_CHNL: return CTYPE_CHNL;
	case BTYPE_DMIX: return CTYPE_DMIX;
	case BTYPE_LUDT: return CTYPE_LUDT;
	case BTYPE_HMHD: return CTYPE_HMHD;
	case BTYPE_INFE: return CTYPE_INFE;
	case BTYPE_SRAT: return CTYPE_SRAT;
	case BTYPE_TXTC: return CTYPE_TXTC;
	case BTYPE_URI: return CTYPE_URI;
	case BTYPE_URII: return CTYPE_URII;
	case BTYPE_STHD: return CTYPE_STHD;
	case BTYPE_MMPU: return CTYPE_MMPU;
	case BTYPE_MULI: return CTYPE_MULI;

	default: return NULL;
	}
}

static bool check_vaild_type(uint32_t btype){
	switch (btype){
	case BTYPE_FTYP: return true;
	case BTYPE_MDAT: return true;
	case BTYPE_MOOV: return true;
	case BTYPE_MVHD: return true;
	case BTYPE_TRAK: return true;
	case BTYPE_TKHD: return true;
	case BTYPE_TREF: return true;
	case BTYPE_TRGR: return true;
	case BTYPE_MDIA: return true;
	case BTYPE_MDHD: return true;
	case BTYPE_HDLR: return true;
	case BTYPE_MINF: return true;
	case BTYPE_ELNG: return true;
	case BTYPE_STBL: return true;
	case BTYPE_STDP: return true;
	case BTYPE_STTS: return true;
	case BTYPE_CTTS: return true;
	case BTYPE_CSLG: return true;
	case BTYPE_STSS: return true;
	case BTYPE_STSH: return true;
	case BTYPE_EDTS: return true;
	case BTYPE_ELST: return true;
	case BTYPE_BTRT: return true;
	case BTYPE_DINF: return true;
	case BTYPE_STSZ: return true;
	case BTYPE_STZ2: return true;
	case BTYPE_STSC: return true;
	case BTYPE_STCO: return true;
	case BTYPE_CO64: return true;
	case BTYPE_PADB: return true;
	case BTYPE_SUBS: return true;
	case BTYPE_SAIZ: return true;
	case BTYPE_SAIO: return true;
	case BTYPE_MVEX: return true;
	case BTYPE_MEHD: return true;
	case BTYPE_TREX: return true;
	case BTYPE_MOOF: return true;
	case BTYPE_MFHD: return true;
	case BTYPE_TRAF: return true;
	case BTYPE_TFHD: return true;
	case BTYPE_TRUN: return true;
	case BTYPE_MFRA: return true;
	case BTYPE_TFRA: return true;
	case BTYPE_MFRO: return true;
	case BTYPE_TFDT: return true;
	case BTYPE_LEVA: return true;
	case BTYPE_TREP: return true;
	case BTYPE_ASSP: return true;
	case BTYPE_SBGP: return true;
	case BTYPE_SGPD: return true;
	case BTYPE_UDTA: return true;
	case BTYPE_CPRT: return true;
	case BTYPE_TSEL: return true;
	case BTYPE_KIND: return true;
	case BTYPE_META: return true;
	case BTYPE_XML: return true;
	case BTYPE_BXML: return true;
	case BTYPE_ILOC: return true;
	case BTYPE_PITM: return true;
	case BTYPE_IPRO: return true;
	case BTYPE_IINF: return true;
	case BTYPE_MECO: return true;
	case BTYPE_MERE: return true;
	case BTYPE_IDAT: return true;
	case BTYPE_IREF: return true;
	case BTYPE_FIIN: return true;
	case BTYPE_FPAR: return true;
	case BTYPE_FECR: return true;
	case BTYPE_SEGR: return true;
	case BTYPE_GITN: return true;
	case BTYPE_FIRE: return true;
	case BTYPE_STRK: return true;
	case BTYPE_STRI: return true;
	case BTYPE_STRD: return true;
	case BTYPE_STSG: return true;
	case BTYPE_STVI: return true;
	case BTYPE_STYP: return true;
	case BTYPE_SIDX: return true;
	case BTYPE_SSIX: return true;
	case BTYPE_PRFT: return true;
	case BTYPE_SRPP: return true;
	case BTYPE_FECI: return true;
	case BTYPE_FREE: return true;
	case BTYPE_SKIP: return true;
	case BTYPE_PDIN: return true;
	case BTYPE_NMHD: return true;
	case BTYPE_STSD: return true;
	case BTYPE_SDTP: return true;
	case BTYPE_DREF: return true;
	case BTYPE_URL: return true;
	case BTYPE_URN: return true;
	case BTYPE_SINF: return true;
	case BTYPE_FRMA: return true;
	case BTYPE_SCHM: return true;
	case BTYPE_SCHI: return true;
	case BTYPE_RINF: return true;
	case BTYPE_CINF: return true;
	case BTYPE_VMHD: return true;
	case BTYPE_SMHD: return true;
	case BTYPE_CHNL: return true;
	case BTYPE_DMIX: return true;
	case BTYPE_LUDT: return true;
	case BTYPE_HMHD: return true;
	case BTYPE_INFE: return true;
	case BTYPE_SRAT: return true;
	case BTYPE_TXTC: return true;
	case BTYPE_URI: return true;
	case BTYPE_URII: return true;
	case BTYPE_STHD: return true;
	case BTYPE_MMPU: return true;
	case BTYPE_MULI: return true;
	default: return false;
	}
}

static bool isFullBox(uint32_t type){
	switch (type)
	{
	case BTYPE_PDIN:return true;
	case BTYPE_MVHD:return true;
	case BTYPE_TKHD:return true;
	case BTYPE_TRGR:return true;
	case BTYPE_MDHD:return true;
	case BTYPE_HDLR:return true;
	case BTYPE_NMHD:return true;
	case BTYPE_ELNG:return true;
	case BTYPE_STSD:return true;
	case BTYPE_STDP:return true;
	case BTYPE_STTS:return true;
	case BTYPE_CTTS:return true;
	case BTYPE_CSLG:return true;
	case BTYPE_STSS:return true;
	case BTYPE_STSH:return true;
	case BTYPE_SDTP:return true;
	case BTYPE_ELST:return true;
	case BTYPE_URL:return true;
	case BTYPE_URN:return true;
	case BTYPE_DREF:return true;
	case BTYPE_STSZ:return true;
	case BTYPE_STZ2:return true;
	case BTYPE_STSC:return true;
	case BTYPE_STCO:return true;
	case BTYPE_CO64:return true;
	case BTYPE_PADB:return true;
	case BTYPE_SUBS:return true;
	case BTYPE_SAIZ:return true;
	case BTYPE_SAIO:return true;
	case BTYPE_MEHD:return true;
	case BTYPE_TREX:return true;
	case BTYPE_MFHD:return true;
	case BTYPE_TFHD:return true;
	case BTYPE_TRUN:return true;
	case BTYPE_TFRA:return true;
	case BTYPE_MFRO:return true;
	case BTYPE_TFDT:return true;
	case BTYPE_LEVA:return true;
	case BTYPE_TREP:return true;
	case BTYPE_ASSP:return true;
	case BTYPE_SBGP:return true;
	case BTYPE_SGPD:return true;
	case BTYPE_CPRT:return true;
	case BTYPE_TSEL:return true;
	case BTYPE_KIND:return true;
	case BTYPE_META:return true;
	case BTYPE_XML:return true;
	case BTYPE_BXML:return true;
	case BTYPE_ILOC:return true;
	case BTYPE_PITM:return true;
	case BTYPE_IPRO:return true;
	case BTYPE_INFE:return true;
	case BTYPE_IINF:return true;
	case BTYPE_MERE:return true;
	case BTYPE_IREF:return true;
	case BTYPE_SCHM:return true;
	case BTYPE_FIIN:return true;
	case BTYPE_FPAR:return true;
	case BTYPE_FECR:return true;
	case BTYPE_GITN:return true;
	case BTYPE_FIRE:return true;
	case BTYPE_STRI:return true;
	case BTYPE_STSG:return true;
	case BTYPE_STVI:return true;
	case BTYPE_SIDX:return true;
	case BTYPE_SSIX:return true;
	case BTYPE_PRFT:return true;
	case BTYPE_SRPP:return true;
	case BTYPE_VMHD:return true;
	case BTYPE_SMHD:return true;
	case BTYPE_SRAT:return true;
	case BTYPE_CHNL:return true;
	case BTYPE_DMIX:return true;
	case BTYPE_LUDT:return true;
	case BTYPE_TXTC:return true;
	case BTYPE_URI:return true;
	case BTYPE_URII:return true;
	case BTYPE_HMHD:return true;
	case BTYPE_STHD:return true;
	case BTYPE_MMPU:return true;
	default:
		return false;
	}
}
