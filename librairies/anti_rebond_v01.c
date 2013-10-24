//-----------------------------------------------------------------------------------//
// Nom du projet 		:	LED_CLIGNO_1B
// Nom du fichier 		:   anti_rebond_v01.c
// Date de création 	:   26.12.2012
// Date de modification : 	24.05.2013
// 
// Auteur 				: 	Philou (Ph Bovey) 
//
// Description 			: 	Ce programme permet de gérer l'antirebond lié à une touche 
//							Programme tiré support de cours ETML - ES --> CH-HUBERT & F-DOMINE
//							chapitre 6 --> introduction au Pics
// Remarques			: 	
// 	chemin pour trouver le headerfile 
//		C:\Program Files\Microchip\MPLAB C30\support\dsPIC33F\h
//
//	doc pour le DSP : 
//		http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en546064
//----------------------------------------------------------------------------------//

//--- appel de librairie ---// 
#include "antirebond.h"

 
//--- déclaration d'enumeration ---//
/*typedef enum
{
	Attente_changement_touche, 		
	Attente_stabilite_touche, 
}Attente_Etat; 

//--- déclaration de strcutures ---//
typedef struct
{
	Attente_Etat Etat_antirepond;
	int compteur; 
	int T_presse; 
	int T_relache; 
	int val_T_presendante; 
	int val_T; 
}Etat_switch; */

//--- déclaration de constantes ---//
const int COMPTEUR_REBOND = 3; 

//--- Déclaration de fonction ---// 
//-----------------------------------------------------//
// - nom fonction 	: antirebond 
// - entrée 		: p_st_Etat, val_in
// - sortie 		: 	 
// - description 	: 
//-----------------------------------------------------//
void antirebond (Etat_switch *p_st_Etat, int val_in)
{
	switch (p_st_Etat->Etat_antirepond)
	{
		//--- mode attente du changement de touche ---//
		case Attente_changement_touche : 
			if( !(val_in == p_st_Etat->val_T_presendante))
			{
				p_st_Etat->val_T_presendante = val_in; 
				p_st_Etat->Etat_antirepond = Attente_stabilite_touche;  
			}
		break;  

		//--- mode attente de satbilité de la touche ---//
		case Attente_stabilite_touche : 
			if(val_in == p_st_Etat->val_T_presendante)
			{
				p_st_Etat->compteur++; 
				if(p_st_Etat->compteur >= COMPTEUR_REBOND)
				{
					p_st_Etat->val_T = val_in; 
					
					if(p_st_Etat->val_T == 0)
						p_st_Etat->T_presse = 1; 
					else 
						p_st_Etat->T_relache = 1; 

					p_st_Etat->Etat_antirepond = Attente_changement_touche; 
					p_st_Etat->compteur = 0; 
				}
			}	
			else 
			{
				p_st_Etat->compteur = 0; 
				p_st_Etat->val_T_presendante = val_in; 
			}
		break;
	} 
}


//-------------------------------------------------------------------//
// - nom fonction 	: init_antirebond
// - entrée 		: p_st_Etat_init
// - sortie 		: 	 
// - description 	: initialisation des paramètre de la structure  
//------------------------------------------------------------------//
void init_antirebond(Etat_switch *p_st_Etat_init)
{
	p_st_Etat_init->compteur = 0; 
	p_st_Etat_init->Etat_antirepond = Attente_changement_touche; 
	p_st_Etat_init->T_presse = 0; 
	p_st_Etat_init->T_relache = 0; 
	p_st_Etat_init->val_T_presendante = 1; 
	p_st_Etat_init->val_T = 1; 
}


//-----------------------------------------------------------//
// - nom fonction 	: etat_switch_apres_rebond 
// - entrée 		: p_st_Etat
// - sortie 		: val_T	 
// - description 	: donner la valeur de la tocuhe après 
//                    anti-rebond 
//-----------------------------------------------------------//
int etat_switch_apres_rebond(Etat_switch *p_st_Etat)
{ 
	return(p_st_Etat->val_T);
}


//-----------------------------------------------------------//
// - nom fonction 	: switch_presser 
// - entrée 		: p_st_Etat
// - sortie 		: T_presse	 
// - description 	: indique si la touche est pressée   
//-----------------------------------------------------------//
int switch_presser(Etat_switch *p_st_Etat)
{
	return(p_st_Etat->T_presse);
}


//-----------------------------------------------------------//
// - nom fonction 	: switch_relacher 
// - entrée 		: p_st_Etat
// - sortie 		: T_relache	 
// - description 	: indique si la touche est relachée  
//-----------------------------------------------------------//
int switch_relacher(Etat_switch *p_st_Etat)
{
	return(p_st_Etat->T_relache);  
}


//-----------------------------------------------------------//
// - nom fonction 	: reset_info_touche_presser 
// - entrée 		: p_st_Etat
// - sortie 		: 	 
// - description 	: réinitialise la valeur de la touche si 
//					  celle-ci pressée   
//-----------------------------------------------------------//
void reset_info_touche_presser(Etat_switch *p_st_Etat)
{
	 p_st_Etat->T_presse = 0;  
}


//-----------------------------------------------------------//
// - nom fonction 	: reset_info_touche_relacher 
// - entrée 		: p_st_Etat
// - sortie 		: 	 
// - description 	: réinitialise la valeur de la touche si 
//					  celle-ci relachée   
//-----------------------------------------------------------//
void reset_info_touche_relacher(Etat_switch *p_st_Etat)
{
	 p_st_Etat->T_presse = 0;  
}












