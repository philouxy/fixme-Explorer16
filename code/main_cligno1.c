//-----------------------------------------------------------------------------------//
// Nom du projet 		:	Cligno_led
// Nom du fichier 		:   main.c
// Date de cr�ation 	:   06.11.2012
// Date de modification : 	21.08.2013
// 
// Auteur 				: 	Philou (Ph. Bovey) 
//
// Description 			: 	Ce programme doit faire clignoter une led 
//
// Modification         :--> modification du timer, valeur de charge (pas de 
//                           d�comptage) 
//
// Remarques			: 	
// 	    chemin pour trouver le headerfile 
//		C:\Program Files\Microchip\MPLAB C30\support\dsPIC33F\h
//
//	    doc pour le DSP : 
//		http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en546064
//----------------------------------------------------------------------------------//
//--- librairie � inclure ---// 
#if defined(__dsPIC33F__)
#include "p33Fxxxx.h"
#elif defined(__PIC24H__)
#include "p24Hxxxx.h"
#endif

//--- configuration des fusibles ---//
_FOSCSEL(FNOSC_PRI); 								// utilisation du quartz de 8MHz externe 
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF  & POSCMD_XT); 	// pin OS2 garder pour l'horlohe 
													// oscillateur XT	
_FWDT(FWDTEN_OFF); 									// ne pas activer le watchdog


//--- prototype de fonction ---//
void init_in_out(void); 							// configuration des entr�es - sorties 
void init_osci(void); 
void init_timer1(void); 							// configuration du timer1; 
void leds_light(void);								// permet d'allumer toutes les leds 


//--- programme principale ---//
void main()
{
	//--- d�sactiver le watchdog ---//
	RCONbits.SWDTEN = 0; 		
 
	//--- initialisation entr�es - sorties ---//
	init_in_out(); 

	//--- initialisation oscillateur ---//
	init_osci(); 

	//--- initialisation timer1 ---//
	init_timer1();

	//--- allumer toutes les leds ---//
	leds_light(); 

	//--- boucle sans fin ---//
	while(1); 
	
}


//--- d�claration de fonction ---//
//----------------------------------------------------------------------------------//
//--- nom 				: init_in_out
//--- entr�e - sortie 	: - / - 
//--- description 		: initilaiser les entr�es et les sorties que l'on d�sire
//						: analogique ou num�rique 
//----------------------------------------------------------------------------------//
void init_in_out(void)
{	
	//--- configuration des sorties num�riques ---//
	TRISAbits.TRISA0 = 0; 
	TRISAbits.TRISA1 = 0; 
	TRISAbits.TRISA2 = 0; 
	TRISAbits.TRISA3 = 0; 
	TRISAbits.TRISA4 = 0; 
	TRISAbits.TRISA5 = 0;
	TRISAbits.TRISA6 = 0;
	TRISAbits.TRISA7 = 0; 
  
} 

//----------------------------------------------------------------------------------//
//--- nom 				: leds_light
//--- entr�e - sortie 	: - / - 
//--- description 		: allume toutes les leds
//----------------------------------------------------------------------------------//
void leds_light(void)
{
	LATAbits.LATA0 = 1; 
	LATAbits.LATA1 = 1;
	LATAbits.LATA2 = 1;
	LATAbits.LATA3 = 1;
	LATAbits.LATA4 = 1;
	LATAbits.LATA5 = 1;
	LATAbits.LATA6 = 1;
	LATAbits.LATA7 = 1;
} 

//----------------------------------------------------------------------------------//
//--- nom 				: init_osci
//--- entr�e - sortie 	: - / - 
//--- description 		: configure l'oscillateur du DSP pour utiliser l'horloge 
//						  � 8MHz
//----------------------------------------------------------------------------------//
init_osci()
{
	//--- configuration de l'oscillateur interne � 40Mhz ---//
	// utilisation fomrules du datasheet du DSPic33FJ256GP710A --> page 146
	// Fcy = Fosc / 2 ; Fosc = Fin(M/(N1 N2))
	// Fin = 8Mhz --> Fosc = 16Mhz --> Fcy = 8Mhz 
	//OSCCONbits.COSC 	= 3;		// s�lection de l'oscillateur XT 
	//OSCCONbits.CLKLOCK	= 0;		// l'horloge et la PLL peuvent �tre modifi� 	
	
	//CLKDIVbits.ROI	= 0; 			// pas d'effet si il y a interruption 
	//CLKDIVbits.DOZE	= 0; 			// pas de r�duction sur l'horloge Fcy /1
	//CLKDIVbits.DOZEN	= 0; 		
	CLKDIVbits.PLLPRE 	= 0; 			// N1 = 2	
    CLKDIVbits.PLLPOST 	= 0; 			// N2 = 2 	
	
	PLLFBDbits.PLLDIV 	= 38;			// M = 40  

	__builtin_write_OSCCONH(0x03); 		// fonction appelant du code assembleur 
										// configuration du registre NOSC (OSCCON) --> 
										// 011 = Primary Oscillator with PLL (XTPLL, HSPLL, ECPLL)
	__builtin_write_OSCCONL(0x01);		// Active la commutation de la clock
	while(OSCCONbits.COSC != 0b011); 	//
}

//----------------------------------------------------------------------------------//
//--- nom 				: init_timer1
//--- entr�e - sortie 	: - / - 
//--- description 		: initilaiser le timer 1 pour avoir une horloge de 500ms 
//----------------------------------------------------------------------------------//
void init_timer1(void)
{
	//--- registre de configuration du Timer1 ---//
	T1CONbits.TON = 0; 			// d�sactiver le timer pour la configuration 
	T1CONbits.TCS = 0; 			// clock interne 
	T1CONbits.TSYNC = 0; 		// synchroniser la clock externe 	
	T1CONbits.TCKPS = 3; 		// pr�divseur r�gler � 256 --> 8Mhz/256 = 31250Hz --> 32us 
	
	TMR1 = 0; 					// mise � z�ro du registre li� au timer 

	PR1 = 15625; 				// p�riode du timer --> 500ms/32us = 15625

	//--- registres des configurations des interruptions li�es au Timer1 ---//
	IPC0bits.T1IP = 7; 			// choix de la priorit� --> ici la plus �lev�e 7 = 111
	IFS0bits.T1IF = 0; 			// remise � z�ro de l'interruption 
	IEC0bits.T1IE = 1; 			// activation de l'interruption li� au Timer1 

	T1CONbits.TON = 1; 			// activer le timer1
}


//----------------------------------------------------------------------------------//
//--- nom 				: _T1Interrupt
//--- entr�e - sortie 	: - / - 
//--- description 		: interruption li� au timer, doit faire clignoter une led
//						: chaque fois qu'on arrive dans cette boucle 
//----------------------------------------------------------------------------------//
void __attribute__((interrupt,no_auto_psv)) _T1Interrupt( void )
{
	IFS0bits.T1IF = 0; 		// remise � z�or du flag d'interruption 

	// prend l'�tat inverse de l'�tat pr�s�dent
	LATAbits.LATA0 = ~LATAbits.LATA0; 	

	TMR1 = 0; 
	T1CONbits.TON = 1;		// activer � nouveau le Timer 
}
