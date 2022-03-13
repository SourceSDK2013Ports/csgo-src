//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "cstrikebuysubmenu.h"
#include "cstrikebuymenu.h"
#include "cs_shareddefs.h"
#include "backgroundpanel.h"
#include "cstrike15/bot/shared_util.h"
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include "cs_gamerules.h"
#include "vgui_controls/RichText.h"
#include "cs_weapon_parse.h"
#include "c_cs_player.h"
#include "cs_ammodef.h"


using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CCSBuyMenu_CT::CCSBuyMenu_CT(IViewPort *pViewPort) : CCSBaseBuyMenu( pViewPort, "BuySubMenu_CT" )
{
	m_pMainMenu->LoadControlSettings( "Resource/UI/BuyMenu_CT.res" );
	m_pMainMenu->SetVisible( false );

	m_iTeam = TEAM_CT;

	CreateBackground( this );
	m_backgroundLayoutFinished = false;
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CCSBuyMenu_TER::CCSBuyMenu_TER(IViewPort *pViewPort) : CCSBaseBuyMenu( pViewPort, "BuySubMenu_TER" )
{
	m_pMainMenu->LoadControlSettings( "Resource/UI/BuyMenu_TER.res" );
	m_pMainMenu->SetVisible( false );

	m_iTeam = TEAM_TERRORIST;

	CreateBackground( this );
	m_backgroundLayoutFinished = false;
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CCSBaseBuyMenu::CCSBaseBuyMenu(IViewPort *pViewPort, const char *subPanelName) : CBuyMenu( pViewPort )
{
	SetTitle( "#Cstrike_Buy_Menu", true);

	SetProportional( true );

	m_pMainMenu = new CCSBuySubMenu( this, subPanelName );
	m_pMainMenu->SetSize( 10, 10 ); // Quiet "parent not sized yet" spew
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCSBaseBuyMenu::SetVisible(bool state)
{
	BaseClass::SetVisible(state);

	if ( state )
	{
		Panel *defaultButton = FindChildByName( "CancelButton" );
		if ( defaultButton )
		{
			defaultButton->RequestFocus();
		}
		SetMouseInputEnabled( true );
		m_pMainMenu->SetMouseInputEnabled( true );
	}
}

//-----------------------------------------------------------------------------
// Purpose: shows/hides the buy menu
//-----------------------------------------------------------------------------
void CCSBaseBuyMenu::ShowPanel(bool bShow)
{
	CBuyMenu::ShowPanel( bShow );
}

//-----------------------------------------------------------------------------
static void GetPanelBounds( Panel *pPanel, wrect_t& bounds )
{
	if ( !pPanel )
	{
		bounds.bottom = bounds.left = bounds.right = bounds.top = 0;
	}
	else
	{
		pPanel->GetBounds( bounds.left, bounds.top, bounds.right, bounds.bottom );
		bounds.right += bounds.left;
		bounds.bottom += bounds.top;
	}
}

//-----------------------------------------------------------------------------
void CCSBaseBuyMenu::Paint()
{
	CBuyMenu::Paint();
}

//-----------------------------------------------------------------------------
// Purpose: The CS background is painted by image panels, so we should do nothing
//-----------------------------------------------------------------------------
void CCSBaseBuyMenu::PaintBackground()
{
}

//-----------------------------------------------------------------------------
// Purpose: Scale / center the window
//-----------------------------------------------------------------------------
void CCSBaseBuyMenu::PerformLayout()
{
	BaseClass::PerformLayout();

	// stretch the window to fullscreen
	if ( !m_backgroundLayoutFinished )
		LayoutBackgroundPanel( this );
	m_backgroundLayoutFinished = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCSBaseBuyMenu::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	ApplyBackgroundSchemeSettings( this, pScheme );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCSBuySubMenu::OnThink()
{
	BaseClass::OnThink();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCSBuySubMenu::OnCommand( const char *command )
{
	if ( FStrEq( command, "buy_unavailable" ) )
	{
		C_CSPlayer *pPlayer = C_CSPlayer::GetLocalCSPlayer();
		if ( pPlayer )
		{
			pPlayer->EmitSound( "BuyPreset.CantBuy" );
		}
		BaseClass::OnCommand( "vguicancel" );
		return;
	}

	BaseClass::OnCommand( command );
}

void CCSBuySubMenu::OnSizeChanged(int newWide, int newTall)
{
	m_backgroundLayoutFinished = false;
	BaseClass::OnSizeChanged( newWide, newTall );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCSBuySubMenu::PerformLayout()
{
	BaseClass::PerformLayout();

	// Buy submenus need to be shoved over for widescreen
	int screenW, screenH;
	GetHudSize( screenW, screenH );

	int fullW, fullH;
	fullW = scheme()->GetProportionalScaledValueEx( GetScheme(), 640 );
	fullH = scheme()->GetProportionalScaledValueEx( GetScheme(), 480 );

	fullW = GetAlternateProportionalValueFromScaled( GetScheme(), fullW );
	fullH = GetAlternateProportionalValueFromScaled( GetScheme(), fullH );

	int offsetX = (screenW - fullW)/2;
	int offsetY = (screenH - fullH)/2;

	if ( !m_backgroundLayoutFinished )
		ResizeWindowControls( this, GetWide(), GetTall(), offsetX, offsetY );
	m_backgroundLayoutFinished = true;
}



