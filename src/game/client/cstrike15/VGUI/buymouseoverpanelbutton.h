//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef BUYMOUSEOVERPANELBUTTON_H
#define BUYMOUSEOVERPANELBUTTON_H
#ifdef _WIN32
#pragma once
#endif

#include <KeyValues.h>
#include <filesystem.h>
#include "mouseoverpanelbutton.h"
#include "hud.h"
#include "c_cs_player.h"
#include "cs_gamerules.h"
#include "cstrike15/bot/shared_util.h"
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/ImagePanel.h>

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Triggers a new panel when the mouse goes over the button
//-----------------------------------------------------------------------------
class BuyMouseOverPanelButton : public MouseOverPanelButton
{
private:
	typedef MouseOverPanelButton BaseClass;
public:
	BuyMouseOverPanelButton(vgui::Panel *parent, const char *panelName, vgui::EditablePanel *panel) :
	  MouseOverPanelButton( parent, panelName, panel)	
	  {
		m_iPrice = 0;
		m_iASRestrict = 0;
		m_iDEUseOnly = 0;
		m_command = NULL;
	  }

	virtual void ApplySettings( KeyValues *resourceData ) 
	{
		BaseClass::ApplySettings( resourceData );

		KeyValues *kv = resourceData->FindKey( "cost", false );
		if( kv ) // if this button has a cost defined for it
		{
			m_iPrice = kv->GetInt(); // save the price away
		}

		kv = resourceData->FindKey( "as_restrict", false );
		if( kv ) // if this button has a map limitation for it
		{
			m_iASRestrict = kv->GetInt(); // save the as_restrict away
		}
		
		kv = resourceData->FindKey( "de_useonly", false );
		if( kv ) // if this button has a map limitation for it
		{
			m_iDEUseOnly = kv->GetInt(); // save the de_useonly away
		}

		if ( m_command )
		{
			delete[] m_command;
			m_command = NULL;
		}
		kv = resourceData->FindKey( "command", false );
		if ( kv )
		{
			m_command = CloneString( kv->GetString() );
		}

		SetPriceState();
		SetMapTypeState();
	}

	int GetASRestrict() { return m_iASRestrict; }

	int GetDEUseOnly() { return m_iDEUseOnly; }

	virtual void PerformLayout()
	{
		BaseClass::PerformLayout();
		SetPriceState();
		SetMapTypeState();

#ifndef CS_SHIELD_ENABLED
		if ( !Q_stricmp( GetName(), "shield" ) )
		{
			SetVisible( false );
			SetEnabled( false );
		}
#endif
	}
	
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme )
	{
		BaseClass::ApplySchemeSettings( pScheme );

		m_avaliableColor = pScheme->GetColor( "Label.TextColor", Color( 0, 0, 0, 0 ) );
		m_unavailableColor = pScheme->GetColor( "Label.DisabledFgColor2", Color( 0, 0, 0, 0 ) );

		SetPriceState();
		SetMapTypeState();
	}

	void SetPriceState()
	{
		if ( GetParent() )
		{
			Panel *pPanel = dynamic_cast< Panel * >(GetParent()->FindChildByName( "MarketSticker" ) ); 

			if ( pPanel )
			{
				pPanel->SetVisible( false );
			}
		}

		C_CSPlayer *pPlayer = C_CSPlayer::GetLocalCSPlayer();

		if ( m_iPrice &&  ( pPlayer && m_iPrice > pPlayer->GetAccount() ) )
		{
			SetFgColor( m_unavailableColor );
			SetCommand( "buy_unavailable" );
		}
		else
		{
			SetFgColor( m_avaliableColor );
			SetCommand( m_command );
		}
	}

	void SetMapTypeState()
	{
		CCSGameRules *pRules = CSGameRules();

		if ( pRules ) 
		{
			if ( !pRules->IsBombDefuseMap() )
			{
				if ( m_iDEUseOnly )
				{
					SetFgColor( m_unavailableColor );
					SetCommand( "buy_unavailable" );
				}
			}
		}
	}

	void SetCurrentPrice( int iPrice )
	{
		m_iPrice = iPrice;
	}

	const char *GetBuyCommand( void )
	{
		return m_command;
	}
	
	virtual void ShowPage()
	{
		if ( g_lastPanel )
		{
			for( int i = 0; i< g_lastPanel->GetParent()->GetChildCount(); i++ )
			{
				MouseOverPanelButton *buyButton = dynamic_cast<MouseOverPanelButton *>(g_lastPanel->GetParent()->GetChild(i));

				if ( buyButton )
				{
					buyButton->HidePage();
				}
			}
		}

		BaseClass::ShowPage();
	}

	virtual void HidePage()
	{
		BaseClass::HidePage();
	}

private:

	int m_iPrice;
	int m_iASRestrict;
	int m_iDEUseOnly;

	Color m_avaliableColor;
	Color m_unavailableColor;

	char *m_command;
};


#endif // BUYMOUSEOVERPANELBUTTON_H
