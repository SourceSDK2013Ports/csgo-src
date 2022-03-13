//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef BASEPANEL_H
#define BASEPANEL_H
#ifdef _WIN32
#pragma once
#endif

#include "vgui_controls/Panel.h"
#include "vgui_controls/PHandle.h"
#include "vgui_controls/MenuItem.h"
#include "vgui_controls/MessageDialog.h"
#include "KeyValues.h"
#include "utlvector.h"
#include "tier1/CommandBuffer.h"

#if !defined( _X360 )
#include "xbox/xboxstubs.h"
#endif

class CMatchmakingBasePanel;
class CBackgroundMenuButton;
class CGameMenu;
class CAsyncCtxOnDeviceAttached;

//-----------------------------------------------------------------------------
// Purpose: Panel that acts as background for button icons and help text in the UI
//-----------------------------------------------------------------------------
class CFooterPanel : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CFooterPanel, vgui::EditablePanel );

public:
	CFooterPanel( Panel *parent, const char *panelName );
	virtual ~CFooterPanel();

	virtual void	ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void	ApplySettings( KeyValues *pResourceData );
	virtual void	Paint( void );
	virtual void	PaintBackground( void );

	// caller tags the current hint, used to assist in ownership
	void			SetHelpNameAndReset( const char *pName );
	const char		*GetHelpName();

	void			AddButtonsFromMap( vgui::Frame *pMenu );
	void			SetStandardDialogButtons();
	void			AddNewButtonLabel( const char *text, const char *icon );
	void			ShowButtonLabel( const char *name, bool show = true );
	void			SetButtonText( const char *buttonName, const char *text );
	void			ClearButtons();
	void			SetButtonGap( int nButtonGap ){ m_nButtonGap = nButtonGap; }
	void			UseDefaultButtonGap(){ m_nButtonGap = m_nButtonGapDefault; }

private:
	struct ButtonLabel_t
	{
		bool	bVisible;
		char	name[MAX_PATH];
		wchar_t	text[MAX_PATH];
		wchar_t	icon[2];			// icon is a single character
	};

	CUtlVector< ButtonLabel_t* > m_ButtonLabels;

	vgui::Label		*m_pSizingLabel;		// used to measure font sizes

	bool			m_bPaintBackground;		// fill the background?
	bool			m_bCenterHorizontal;	// center buttons horizontally?
	int				m_ButtonPinRight;		// if not centered, this is the distance from the right margin that we use to start drawing buttons (right to left)
	int				m_nButtonGap;			// space between buttons when drawing
	int				m_nButtonGapDefault;		// space between buttons (initial value)
	int				m_FooterTall;			// height of the footer
	int				m_ButtonOffsetFromTop;	// how far below the top the buttons should be drawn
	int				m_ButtonSeparator;		// space between the button icon and text
	int				m_TextAdjust;			// extra adjustment for the text (vertically)...text is centered on the button icon and then this value is applied

	char			m_szTextFont[64];		// font for the button text
	char			m_szButtonFont[64];		// font for the button icon
	char			m_szFGColor[64];		// foreground color (text)
	char			m_szBGColor[64];		// background color (fill color)
	
	vgui::HFont		m_hButtonFont;
	vgui::HFont		m_hTextFont;
	char			*m_pHelpName;
};

//-----------------------------------------------------------------------------
// Purpose: EditablePanel that can replace the GameMenuButtons in CBasePanel
//-----------------------------------------------------------------------------
class CMainMenuGameLogo : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CMainMenuGameLogo, vgui::EditablePanel );
public:
	CMainMenuGameLogo( vgui::Panel *parent, const char *name );

	virtual void ApplySettings( KeyValues *inResourceData );
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );

	int GetOffsetX(){ return m_nOffsetX; }
	int GetOffsetY(){ return m_nOffsetY; }

private:
	int m_nOffsetX;
	int m_nOffsetY;
};

//-----------------------------------------------------------------------------
// Purpose: Transparent menu item designed to sit on the background ingame
//-----------------------------------------------------------------------------
class CGameMenuItem : public vgui::MenuItem
{
	DECLARE_CLASS_SIMPLE( CGameMenuItem, vgui::MenuItem );
public:
	CGameMenuItem(vgui::Menu *parent, const char *name);

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void PaintBackground( void );
	void SetRightAlignedText( bool state );

private:
	bool		m_bRightAligned;
};

//-----------------------------------------------------------------------------
// Purpose: This is the panel at the top of the panel hierarchy for GameUI
//			It handles all the menus, background images, and loading dialogs
//-----------------------------------------------------------------------------
class CBasePanel : public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CBasePanel, vgui::Panel );

public:
	CBasePanel();
	virtual ~CBasePanel();

public:
	//
	// Implementation of async jobs
	//	An async job is enqueued by calling "ExecuteAsync" with the proper job context.
	//	Job's function "ExecuteAsync" is called on a separate thread.
	//	After the job finishes the "Completed" function is called on the
	//	main thread.
	//
	class CAsyncJobContext
	{
	public:
		CAsyncJobContext( float flLeastExecuteTime = 0.0f ) : m_flLeastExecuteTime( flLeastExecuteTime ), m_hThreadHandle( NULL )  {}
		virtual ~CAsyncJobContext() {}

		virtual void ExecuteAsync() = 0;		// Executed on the secondary thread
		virtual void Completed() = 0;			// Executed on the main thread

	public:
		void * volatile m_hThreadHandle;		// Handle to an async job thread waiting for
		float m_flLeastExecuteTime;				// Least amount of time this job should keep executing
	};

	CAsyncJobContext *m_pAsyncJob;
	void ExecuteAsync( CAsyncJobContext *pAsync );


public:
	// notifications
	void OnLevelLoadingStarted();
	void OnLevelLoadingFinished();

	// update the taskbar a frame
	void RunFrame();

	// fades to black then runs an engine command (usually to start a level)
	void FadeToBlackAndRunEngineCommand( const char *engineCommand );

	// sets the blinking state of a menu item
	void SetMenuItemBlinkingState( const char *itemName, bool state );

	// handles gameUI being shown
	void OnGameUIActivated();

	// game dialogs
	void OnOpenNewGameDialog( const char *chapter = NULL );
	void OnOpenBonusMapsDialog();
	void OnOpenLoadGameDialog();
	void OnOpenSaveGameDialog();
	void OnOpenServerBrowser();
	void OnOpenFriendsDialog();
	void OnOpenDemoDialog();
	void OnOpenCreateMultiplayerGameDialog();
	void OnOpenQuitConfirmationDialog();
	void OnOpenChangeGameDialog();
	void OnOpenPlayerListDialog();
	void OnOpenBenchmarkDialog();
	void OnOpenOptionsDialog();
	void OnOpenLoadCommentaryDialog();
	void OpenLoadSingleplayerCommentaryDialog();

	KeyValues *GetConsoleControlSettings( void );

	// forces any changed options dialog settings to be applied immediately, if it's open
	void ApplyOptionsDialogSettings();

	vgui::AnimationController *GetAnimationController( void ) { return m_pConsoleAnimationController; }
	void RunCloseAnimation( const char *animName );
	void RunAnimationWithCallback( vgui::Panel *parent, const char *animName, KeyValues *msgFunc );
	void PositionDialog( vgui::PHandle dlg );

	virtual void OnSizeChanged( int newWide, int newTall );

	void ArmFirstMenuItem( void );

	void OnGameUIHidden();

	void CloseBaseDialogs( void );

#if defined( _X360 )
	CON_COMMAND_MEMBER_F( CBasePanel, "gameui_reload_resources", Reload_Resources, "Reload the Xbox 360 UI res files", 0 );
#endif

	int  GetMenuAlpha( void );

	void SetMainMenuOverride( vgui::VPANEL panel );



protected:
	virtual void PaintBackground();
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

private:
	enum EBackgroundState
	{
		BACKGROUND_INITIAL,
		BACKGROUND_LOADING,
		BACKGROUND_MAINMENU,
		BACKGROUND_LEVEL,
		BACKGROUND_DISCONNECTED,
		BACKGROUND_EXITING,			// Console has started an exiting state, cannot be stopped
	};
	void SetBackgroundRenderState(EBackgroundState state);

	void UpdateBackgroundState();

	// sets the menu alpha [0..255]
	void SetMenuAlpha(int alpha);

	// menu manipulation
	void CreatePlatformMenu();
	void CreateGameMenu();
	void CreateGameLogo();
	void CheckBonusBlinkState();
	void UpdateGameMenus();
	CGameMenu *RecursiveLoadGameMenu(KeyValues *datafile);

	void StartExitingProcess();

	bool IsPromptableCommand( const char *command );
	bool CommandRequiresSignIn( const char *command );
	bool CommandRequiresStorageDevice( const char *command );
	bool CommandRespectsSignInDenied( const char *command );

	void QueueCommand( const char *pCommand );
	void RunQueuedCommands();
	void ClearQueuedCommands();

	virtual void OnCommand(const char *command);
	virtual void PerformLayout();
	MESSAGE_FUNC_INT( OnActivateModule, "ActivateModule", moduleIndex);

	void UpdateRichPresenceInfo();

	// menu logo
	CMainMenuGameLogo *m_pGameLogo;
	
	// menu buttons
	CUtlVector< CBackgroundMenuButton * >m_pGameMenuButtons;
	CGameMenu *m_pGameMenu;
	bool m_bPlatformMenuInitialized;
	int m_iGameMenuInset;
	
	vgui::VPANEL	m_hMainMenuOverridePanel;

	struct coord {
		int x;
		int y;
	};
	CUtlVector< coord > m_iGameTitlePos;
	coord m_iGameMenuPos;

	// base dialogs
	vgui::DHANDLE<vgui::Frame> m_hNewGameDialog;
	vgui::DHANDLE<vgui::Frame> m_hBonusMapsDialog;
	vgui::DHANDLE<vgui::Frame> m_hLoadGameDialog;
	vgui::DHANDLE<vgui::Frame> m_hSaveGameDialog;
	vgui::DHANDLE<vgui::PropertyDialog> m_hOptionsDialog;
	vgui::DHANDLE<vgui::Frame> m_hCreateMultiplayerGameDialog;
	//vgui::DHANDLE<vgui::Frame> m_hDemoPlayerDialog;
	vgui::DHANDLE<vgui::Frame> m_hChangeGameDialog;
	vgui::DHANDLE<vgui::Frame> m_hPlayerListDialog;
	vgui::DHANDLE<vgui::Frame> m_hBenchmarkDialog;
	vgui::DHANDLE<vgui::Frame> m_hLoadCommentaryDialog;

	EBackgroundState m_eBackgroundState;

	CUtlVector< CUtlString >	m_CommandQueue;

	vgui::AnimationController	*m_pConsoleAnimationController;
	KeyValues					*m_pConsoleControlSettings;

	void						DrawBackgroundImage();
	int							m_iBackgroundImageID;
	int							m_iRenderTargetImageID;
	int							m_iLoadingImageID;
	int							m_iProductImageID;
	bool						m_bLevelLoading;
	bool						m_bEverActivated;
	bool						m_bCopyFrameBuffer;
	bool						m_bUseRenderTargetImage;
	int							m_ExitingFrameCount;
	bool						m_bXUIVisible;
	bool						m_bUseMatchmaking;
	bool						m_bRestartFromInvite;
	bool						m_bRestartSameGame;

	// background transition
	bool m_bFadingInMenus;
	float m_flFadeMenuStartTime;
	float m_flFadeMenuEndTime;

	bool m_bRenderingBackgroundTransition;
	float m_flTransitionStartTime;
	float m_flTransitionEndTime;

	// background fill transition
	bool m_bHaveDarkenedBackground;
	bool m_bHaveDarkenedTitleText;
	bool m_bForceTitleTextUpdate;
	float m_flFrameFadeInTime;
	Color m_BackdropColor;
	CPanelAnimationVar( float, m_flBackgroundFillAlpha, "m_flBackgroundFillAlpha", "0" );

	// fading to game
	MESSAGE_FUNC_CHARPTR( RunEngineCommand, "RunEngineCommand", command );
	MESSAGE_FUNC( FinishDialogClose, "FinishDialogClose" );

public:
	MESSAGE_FUNC_CHARPTR( RunMenuCommand, "RunMenuCommand", command );
};

//-----------------------------------------------------------------------------
// Purpose: singleton accessor
//-----------------------------------------------------------------------------
extern CBasePanel *BasePanel();


#endif // BASEPANEL_H
