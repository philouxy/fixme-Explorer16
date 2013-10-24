//-----------------------------------------------------------------------------------//
// Nom du projet 		:	AFFICHAGE_SIMPLE
// Nom du fichier 		:   main_affichage_simple.c
// Date de création 	:   21.08.2013
// Date de modification : 	xx.xx.2013
// 
// Auteur 				: 	Philou (Ph Bovey)
//
// Description 			: 	Ce programme doit afficher un message écrit dans le main
//							sur le LCD de la carte exploreur 16 
//
// Modification 		:   - 
//
// Remarques			: 	
// 	chemin pour trouver le headerfile 
//		C:\Program Files\Microchip\MPLAB C30\support\dsPIC33F\h
//
//	doc pour le DSP : 
//		http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en546064
//  doc pour la carte explorer : 
//     	http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&nodeId=1406&dDocName=en024858&part=dm240001&redirects=explorer16
//----------------------------------------------------------------------------------//
//--- librairie à inclure ---// 
#if defined(__dsPIC33F__)
#include "p33Fxxxx.h"
#elif defined(__PIC24H__)
#include "p24Hxxxx.h"
#endif

//#include <stdbool.h>
//#include "antirebond.h"
#include "lcd.h"

//--- configuration des fusibles ---//
_FOSCSEL(FNOSC_PRI); 								// utilisation du quartz de 8MHz externe 
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF  & POSCMD_XT); 	// pin OS2 garder pour l'horlohe 
													// oscillateur XT	
_FWDT(FWDTEN_OFF); 									// ne pas activer le watchdog

void main()
{
	Init_LCD(); 		// initialisation de l'affichage 
	lcd_cmd(0x01); 		//#define home_clr()      lcd_cmd( 0x01 )


	//--- texte de test ---// 
	lcd_data('T');
	//lcd_cmd(0x14);  						//#define cursor_right()  lcd_cmd( 0x14 )
	lcd_data('E');
	//lcd_cmd(0x14);
	lcd_data('S');
	//lcd_cmd(0x14);
	lcd_data('T');
	//lcd_cmd(0x14);

	lcd_cmd(0xC0); 							//#define line_2()        lcd_cmd( 0xC0 )

	lcd_data('F');
	lcd_data('i');
	lcd_data('x');
	lcd_data('M');
	lcd_data('e');

	lcd_cmd(0x14);

	lcd_data(0xFF);
	lcd_cmd(0x0F);  
}







