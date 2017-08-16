//
// This file is released under the terms of the NASA Open Source Agreement (NOSA)
// version 1.3 as detailed in the LICENSE file which accompanies this software.
//

// FeaPartEditScreen.cpp FeaPart editor screen.
// Justin Gravett
//
//////////////////////////////////////////////////////////////////////

#include "FeaPartEditScreen.h"
#include "StructureMgr.h"
#include "SubSurface.h"
#include "FeaMeshMgr.h"

FeaPartEditScreen::FeaPartEditScreen( ScreenMgr* mgr ) : BasicScreen( mgr, 310, 460, "FEA Part Edit" )
{
    m_FLTK_Window->callback( staticCloseCB, this );

    //=== FEA Parts ===//
    m_GenLayout.SetGroupAndScreen( m_FLTK_Window, this );

    m_GenLayout.AddY( m_GenLayout.GetStdHeight() + m_GenLayout.GetGapHeight() );

    m_GenLayout.AddInput( m_FeaPartNameInput, "Part Name" );

    m_GenLayout.AddYGap();

    m_CurFeaPartDispGroup = NULL;

    // Indivdual FeaPart Parameters:
    int start_y = m_GenLayout.GetY();

    //==== FeaSlice ====//
    m_GenLayout.AddSubGroupLayout( m_SkinEditLayout, m_GenLayout.GetW(), m_GenLayout.GetH() );
    m_SkinEditLayout.SetY( start_y );

    m_SkinEditLayout.AddDividerBox( "Skin" );

    m_SkinEditLayout.AddButton( m_RemoveSkinTrisToggle, "Remove Skin Tris" );

    m_SkinEditLayout.AddYGap();

    m_SkinEditLayout.AddChoice( m_SkinPropertyChoice, "Property" );

    //==== FeaSlice ====//
    m_GenLayout.AddSubGroupLayout( m_SliceEditLayout, m_GenLayout.GetW(), m_GenLayout.GetH() );
    m_SliceEditLayout.SetY( start_y );

    m_SliceEditLayout.AddDividerBox( "Slice" );

    m_SliceEditLayout.SetButtonWidth( m_SliceEditLayout.GetRemainX() / 3 );
    m_SliceEditLayout.SetChoiceButtonWidth( m_SliceEditLayout.GetRemainX() / 3 );

    m_SliceOrientationChoice.AddItem( "XY_Body" );
    m_SliceOrientationChoice.AddItem( "YZ_Body" );
    m_SliceOrientationChoice.AddItem( "XZ_Body" );
    m_SliceOrientationChoice.AddItem( "XY_Abs" );
    m_SliceOrientationChoice.AddItem( "YZ_Abs" );
    m_SliceOrientationChoice.AddItem( "XZ_Abs" );
    m_SliceOrientationChoice.AddItem( "Const_U" );
    m_SliceEditLayout.AddChoice( m_SliceOrientationChoice, "Orientation" );

    m_SliceEditLayout.SetSameLineFlag( true );
    m_SliceEditLayout.SetFitWidthFlag( false );

    int button_width = m_SliceEditLayout.GetButtonWidth();
    int slider_width = m_SliceEditLayout.GetSliderWidth();

    m_SliceEditLayout.SetSliderWidth( button_width );
    m_SliceEditLayout.SetButtonWidth( 0 );
    m_SliceEditLayout.SetChoiceButtonWidth( 0 );

    m_SlicePosTypeChoice.AddItem( "Percent" );
    m_SlicePosTypeChoice.AddItem( "Distance" );
    m_SliceEditLayout.AddChoice( m_SlicePosTypeChoice, "" );

    m_SliceEditLayout.SetSliderWidth( slider_width - 5 );

    m_SliceEditLayout.AddSlider( m_SliceCenterLocSlider, " ", 50, "%5.3f" );

    m_SliceEditLayout.AddButton( m_SlicePosUnit, "" );
    m_SlicePosUnit.GetFlButton()->box( FL_THIN_UP_BOX );
    m_SlicePosUnit.GetFlButton()->labelcolor( FL_BLACK );
    m_SlicePosUnit.SetWidth( m_SliceEditLayout.GetRemainX() );

    m_SliceEditLayout.ForceNewLine();
    m_SliceEditLayout.AddYGap();

    m_SliceEditLayout.InitWidthHeightVals();

    m_SliceEditLayout.AddLabel( "Rotation Axis", m_SliceEditLayout.GetRemainX() / 3 );

    m_SliceEditLayout.SetButtonWidth( m_SliceEditLayout.GetRemainX() / 3 );

    m_SliceEditLayout.AddButton( m_SliceXAxisToggle, "X Axis" );
    m_SliceEditLayout.AddButton( m_SliceYAxisToggle, "Y Axis" );
    m_SliceEditLayout.AddButton( m_SliceZAxisToggle, "Z Axis" );

    m_SliceRotAxisToggleGroup.Init( this );
    m_SliceRotAxisToggleGroup.AddButton( m_SliceXAxisToggle.GetFlButton() );
    m_SliceRotAxisToggleGroup.AddButton( m_SliceYAxisToggle.GetFlButton() );
    m_SliceRotAxisToggleGroup.AddButton( m_SliceZAxisToggle.GetFlButton() );

    m_SliceEditLayout.ForceNewLine();
    m_SliceEditLayout.SetSameLineFlag( false );
    m_SliceEditLayout.SetFitWidthFlag( true );

    m_SliceEditLayout.SetButtonWidth( button_width );

    m_SliceEditLayout.AddSlider( m_SliceXRotSlider, "XRot", 25, "%5.3f" );
    m_SliceEditLayout.AddSlider( m_SliceYRotSlider, "YRot", 25, "%5.3f" );
    m_SliceEditLayout.AddSlider( m_SliceZRotSlider, "ZRot", 25, "%5.3f" );

    m_SliceEditLayout.AddYGap();

    m_SliceEditLayout.SetSameLineFlag( true );
    m_SliceEditLayout.SetFitWidthFlag( false );

    m_SliceEditLayout.SetButtonWidth( m_SliceEditLayout.GetRemainX() / 3 );
    m_SliceEditLayout.AddButton( m_SliceShellToggle, "Shell" );
    m_SliceEditLayout.AddButton( m_SliceCapToggle, "Cap" );
    m_SliceEditLayout.AddButton( m_SliceShellCapToggle, "Shell and Cap" );

    m_SliceShellCapToggleGroup.Init( this );
    m_SliceShellCapToggleGroup.AddButton( m_SliceShellToggle.GetFlButton() );
    m_SliceShellCapToggleGroup.AddButton( m_SliceCapToggle.GetFlButton() );
    m_SliceShellCapToggleGroup.AddButton( m_SliceShellCapToggle.GetFlButton() );

    m_SliceEditLayout.SetSameLineFlag( false );
    m_SliceEditLayout.SetFitWidthFlag( true );
    m_SliceEditLayout.ForceNewLine();
    m_SliceEditLayout.SetChoiceButtonWidth( m_SliceEditLayout.GetRemainX() / 3 );

    m_SliceEditLayout.AddChoice( m_SlicePropertyChoice, "Property" );
    m_SliceEditLayout.AddChoice( m_SliceCapPropertyChoice, "Cap Property" );

    //==== FeaRib ====//
    m_GenLayout.AddSubGroupLayout( m_RibEditLayout, m_GenLayout.GetW(), m_GenLayout.GetH() );
    m_RibEditLayout.SetY( start_y );

    m_RibEditLayout.AddDividerBox( "Rib" );

    m_RibEditLayout.SetButtonWidth( m_RibEditLayout.GetRemainX() / 3 );
    m_RibEditLayout.SetChoiceButtonWidth( m_RibEditLayout.GetRemainX() / 3 );

    m_RibEditLayout.AddChoice( m_RibPerpendicularEdgeChoice, "Edge Normal" );

    m_RibEditLayout.SetSameLineFlag( true );
    m_RibEditLayout.SetFitWidthFlag( false );

    button_width = m_RibEditLayout.GetButtonWidth();
    slider_width = m_RibEditLayout.GetSliderWidth();

    m_RibEditLayout.SetSliderWidth( button_width );
    m_RibEditLayout.SetButtonWidth( 0 );
    m_RibEditLayout.SetChoiceButtonWidth( 0 );

    m_RibPosTypeChoice.AddItem( "% Span" );
    m_RibPosTypeChoice.AddItem( "Dist Span" );
    m_RibEditLayout.AddChoice( m_RibPosTypeChoice, "" );

    m_RibEditLayout.SetSliderWidth( slider_width - 5 );

    m_RibEditLayout.AddSlider( m_RibPosSlider, " ", 50, "%5.3f" );

    m_RibEditLayout.AddButton( m_RibPosUnit, "" );
    m_RibPosUnit.GetFlButton()->box( FL_THIN_UP_BOX );
    m_RibPosUnit.GetFlButton()->labelcolor( FL_BLACK );
    m_RibPosUnit.SetWidth( m_RibEditLayout.GetRemainX() );

    m_RibEditLayout.ForceNewLine();
    m_RibEditLayout.AddYGap();

    m_RibEditLayout.InitWidthHeightVals();
    m_RibEditLayout.SetSameLineFlag( false );
    m_RibEditLayout.SetFitWidthFlag( true );

    m_RibEditLayout.SetButtonWidth( button_width );

    m_RibEditLayout.AddSlider( m_RibThetaSlider, "Rotation", 25, "%5.3f" );

    m_RibEditLayout.AddYGap();

    m_RibEditLayout.SetSameLineFlag( true );
    m_RibEditLayout.SetFitWidthFlag( false );

    m_RibEditLayout.SetButtonWidth( m_RibEditLayout.GetRemainX() / 3 );
    m_RibEditLayout.AddButton( m_RibShellToggle, "Shell" );
    m_RibEditLayout.AddButton( m_RibCapToggle, "Cap" );
    m_RibEditLayout.AddButton( m_RibShellCapToggle, "Shell and Cap" );

    m_RibShellCapToggleGroup.Init( this );
    m_RibShellCapToggleGroup.AddButton( m_RibShellToggle.GetFlButton() );
    m_RibShellCapToggleGroup.AddButton( m_RibCapToggle.GetFlButton() );
    m_RibShellCapToggleGroup.AddButton( m_RibShellCapToggle.GetFlButton() );

    m_RibEditLayout.ForceNewLine();
    m_RibEditLayout.SetSameLineFlag( false );
    m_RibEditLayout.SetFitWidthFlag( true );
    m_RibEditLayout.SetChoiceButtonWidth( m_RibEditLayout.GetRemainX() / 3 );

    m_RibEditLayout.AddChoice( m_RibPropertyChoice, "Property" );
    m_RibEditLayout.AddChoice( m_RibCapPropertyChoice, "Cap Property" );

    //==== FeaSpar ====//
    m_GenLayout.AddSubGroupLayout( m_SparEditLayout, m_GenLayout.GetW(), m_GenLayout.GetH() );
    m_SparEditLayout.SetY( start_y );

    m_SparEditLayout.AddDividerBox( "Spar" );

    m_SparEditLayout.SetButtonWidth( m_SparEditLayout.GetRemainX() / 2 );

    int input_width = m_SparEditLayout.GetInputWidth();

    m_SparEditLayout.SetSameLineFlag( true );
    m_SparEditLayout.SetFitWidthFlag( false );

    m_SparEditLayout.AddButton( m_SparSectionLimitToggle, "Limit Spar to Section" );

    m_SparEditLayout.SetButtonWidth( m_SparEditLayout.GetRemainX() / 4 );
    m_SparEditLayout.SetInputWidth( m_SparEditLayout.GetRemainX() / 5 );

    m_SparEditLayout.AddIndexSelector( m_SparSectIndexSelector );

    m_SparEditLayout.ForceNewLine();

    button_width = m_SparEditLayout.GetRemainX() / 3;
    slider_width = m_SparEditLayout.GetSliderWidth();

    m_SparEditLayout.SetSliderWidth( button_width );
    m_SparEditLayout.SetInputWidth( input_width );
    m_SparEditLayout.SetButtonWidth( 0 );
    m_SparEditLayout.SetChoiceButtonWidth( 0 );

    m_SparPosTypeChoice.AddItem( "% Chord" );
    m_SparPosTypeChoice.AddItem( "Dist Chord" );
    m_SparEditLayout.AddChoice( m_SparPosTypeChoice, "" );

    m_SparEditLayout.SetSliderWidth( slider_width - 5 );

    m_SparEditLayout.AddSlider( m_SparPosSlider, " ", 50, "%5.3f" );

    m_SparEditLayout.AddButton( m_SparPosUnit, "" );
    m_SparPosUnit.GetFlButton()->box( FL_THIN_UP_BOX );
    m_SparPosUnit.GetFlButton()->labelcolor( FL_BLACK );
    m_SparPosUnit.SetWidth( m_SparEditLayout.GetRemainX() );

    m_SparEditLayout.ForceNewLine();
    m_SparEditLayout.AddYGap();

    m_SparEditLayout.InitWidthHeightVals();
    m_SparEditLayout.SetSameLineFlag( false );
    m_SparEditLayout.SetFitWidthFlag( true );

    m_SparEditLayout.SetButtonWidth( button_width );

    m_SparEditLayout.AddSlider( m_SparThetaSlider, "Rotation", 25, "%5.3f" );

    m_SparEditLayout.AddYGap();

    m_SparEditLayout.SetSameLineFlag( true );
    m_SparEditLayout.SetFitWidthFlag( false );

    m_SparEditLayout.SetButtonWidth( m_SparEditLayout.GetRemainX() / 3 );
    m_SparEditLayout.AddButton( m_SparShellToggle, "Shell" );
    m_SparEditLayout.AddButton( m_SparCapToggle, "Cap" );
    m_SparEditLayout.AddButton( m_SparShellCapToggle, "Shell and Cap" );

    m_SparShellCapToggleGroup.Init( this );
    m_SparShellCapToggleGroup.AddButton( m_SparShellToggle.GetFlButton() );
    m_SparShellCapToggleGroup.AddButton( m_SparCapToggle.GetFlButton() );
    m_SparShellCapToggleGroup.AddButton( m_SparShellCapToggle.GetFlButton() );

    m_SparEditLayout.SetSameLineFlag( false );
    m_SparEditLayout.SetFitWidthFlag( true );
    m_SparEditLayout.ForceNewLine();
    m_SparEditLayout.SetChoiceButtonWidth( m_SparEditLayout.GetRemainX() / 3 );

    m_SparEditLayout.AddChoice( m_SparPropertyChoice, "Property" );
    m_SparEditLayout.AddChoice( m_SparCapPropertyChoice, "Cap Property" );

    //==== FeaFixPoint ====//
    m_GenLayout.AddSubGroupLayout( m_FixPointEditLayout, m_GenLayout.GetW(), m_GenLayout.GetH() );
    m_FixPointEditLayout.SetY( start_y );

    m_FixPointEditLayout.AddDividerBox( "Fixed Point" );

    m_FixPointEditLayout.SetChoiceButtonWidth( m_FixPointEditLayout.GetRemainX() / 3 );
    m_FixPointEditLayout.SetButtonWidth( m_FixPointEditLayout.GetRemainX() / 3 );

    m_FixPointEditLayout.AddChoice( m_FixPointParentSurfChoice, "Parent Surface" );

    m_FixPointEditLayout.AddSlider( m_FixPointULocSlider, "U Location", 1, "%5.3f" );
    m_FixPointEditLayout.AddSlider( m_FixPointWLocSlider, "W Location", 1, "%5.3f" );

    m_FixPointEditLayout.AddYGap();

    m_FixPointEditLayout.SetSameLineFlag( true );
    m_FixPointEditLayout.SetFitWidthFlag( false );

    m_FixPointEditLayout.SetButtonWidth( m_FixPointEditLayout.GetRemainX() / 17 );

    m_FixPointEditLayout.AddButton( m_FixPointMassToggle, "" );

    m_FixPointEditLayout.SetButtonWidth( ( m_FixPointEditLayout.GetW() / 3 ) - ( m_FixPointEditLayout.GetRemainX() / 16 ) );
    m_FixPointEditLayout.SetSliderWidth( m_FixPointEditLayout.GetW() / 4 );
    m_FixPointEditLayout.SetInputWidth(  m_FixPointEditLayout.GetRemainX() / 6 );

    m_FixPointEditLayout.AddSlider( m_FixPointMassSlider, "Mass", 100.0, "%5.3g" );

    m_FixPointEditLayout.AddButton( m_FixPointMassUnit, "" );
    m_FixPointMassUnit.GetFlButton()->box( FL_THIN_UP_BOX );
    m_FixPointMassUnit.GetFlButton()->labelcolor( FL_BLACK );
    m_FixPointMassUnit.SetWidth( ( m_FixPointEditLayout.GetW() / 5 ) );

    //==== FeaDome ====//
    m_GenLayout.AddSubGroupLayout( m_DomeEditLayout, m_GenLayout.GetW(), m_GenLayout.GetH() );
    m_DomeEditLayout.SetY( start_y );

    m_DomeEditLayout.AddDividerBox( "Dome" );

    m_DomeEditLayout.SetSameLineFlag( true );
    m_DomeEditLayout.SetFitWidthFlag( false );

    m_DomeEditLayout.SetButtonWidth( m_DomeEditLayout.GetRemainX() / 3 );
    m_DomeEditLayout.SetSliderWidth( m_DomeEditLayout.GetSliderWidth() - 5 );

    m_DomeEditLayout.AddSlider( m_DomeARadSlider, "A Radius", 10, "%5.3f" );

    m_DomeEditLayout.AddButton( m_DomeARadUnit, " " );
    m_DomeARadUnit.GetFlButton()->box( FL_THIN_UP_BOX );
    m_DomeARadUnit.GetFlButton()->labelcolor( FL_BLACK );
    m_DomeARadUnit.SetWidth( m_DomeEditLayout.GetW() / 14 );

    m_DomeEditLayout.ForceNewLine();

    m_DomeEditLayout.AddSlider( m_DomeBRadSlider, "B Radius", 10, "%5.3f" );

    m_DomeEditLayout.AddButton( m_DomeBRadUnit, " " );
    m_DomeBRadUnit.GetFlButton()->box( FL_THIN_UP_BOX );
    m_DomeBRadUnit.GetFlButton()->labelcolor( FL_BLACK );
    m_DomeBRadUnit.SetWidth( m_DomeEditLayout.GetW() / 14 );

    m_DomeEditLayout.ForceNewLine();

    m_DomeEditLayout.AddSlider( m_DomeCRadSlider, "C Radius", 10, "%5.3f" );

    m_DomeEditLayout.AddButton( m_DomeCRadUnit, " " );
    m_DomeCRadUnit.GetFlButton()->box( FL_THIN_UP_BOX );
    m_DomeCRadUnit.GetFlButton()->labelcolor( FL_BLACK );
    m_DomeCRadUnit.SetWidth( m_DomeEditLayout.GetW() / 14 );

    m_DomeEditLayout.ForceNewLine();
    m_DomeEditLayout.AddYGap();

    m_DomeEditLayout.AddSlider( m_DomeXSlider, "XLoc", 50, "%5.3f" );

    m_DomeEditLayout.AddButton( m_DomeXUnit, " " );
    m_DomeXUnit.GetFlButton()->box( FL_THIN_UP_BOX );
    m_DomeXUnit.GetFlButton()->labelcolor( FL_BLACK );
    m_DomeXUnit.SetWidth( m_DomeEditLayout.GetW() / 14 );

    m_DomeEditLayout.ForceNewLine();

    m_DomeEditLayout.AddSlider( m_DomeYSlider, "YLoc", 50, "%5.3f" );

    m_DomeEditLayout.AddButton( m_DomeYUnit, " " );
    m_DomeYUnit.GetFlButton()->box( FL_THIN_UP_BOX );
    m_DomeYUnit.GetFlButton()->labelcolor( FL_BLACK );
    m_DomeYUnit.SetWidth( m_DomeEditLayout.GetW() / 14 );

    m_DomeEditLayout.ForceNewLine();

    m_DomeEditLayout.AddSlider( m_DomeZSlider, "ZLoc", 50, "%5.3f" );

    m_DomeEditLayout.AddButton( m_DomeZUnit, " " );
    m_DomeZUnit.GetFlButton()->box( FL_THIN_UP_BOX );
    m_DomeZUnit.GetFlButton()->labelcolor( FL_BLACK );
    m_DomeZUnit.SetWidth( m_DomeEditLayout.GetW() / 14 );

    m_DomeEditLayout.ForceNewLine();
    m_DomeEditLayout.AddYGap();

    m_DomeEditLayout.SetSameLineFlag( false );
    m_DomeEditLayout.SetFitWidthFlag( true );

    m_DomeEditLayout.AddSlider( m_DomeXRotSlider, "XRot", 25, "%5.3f" );
    m_DomeEditLayout.AddSlider( m_DomeYRotSlider, "YRot", 25, "%5.3f" );
    m_DomeEditLayout.AddSlider( m_DomeZRotSlider, "ZRot", 25, "%5.3f" );

    m_DomeEditLayout.AddYGap();

    m_DomeEditLayout.SetSameLineFlag( true );
    m_DomeEditLayout.SetFitWidthFlag( false );

    m_DomeEditLayout.SetButtonWidth( m_DomeEditLayout.GetRemainX() / 3 );
    m_DomeEditLayout.AddButton( m_DomeShellToggle, "Shell" );
    m_DomeEditLayout.AddButton( m_DomeCapToggle, "Cap" );
    m_DomeEditLayout.AddButton( m_DomeShellCapToggle, "Shell and Cap" );

    m_DomeShellCapToggleGroup.Init( this );
    m_DomeShellCapToggleGroup.AddButton( m_DomeShellToggle.GetFlButton() );
    m_DomeShellCapToggleGroup.AddButton( m_DomeCapToggle.GetFlButton() );
    m_DomeShellCapToggleGroup.AddButton( m_DomeShellCapToggle.GetFlButton() );

    m_DomeEditLayout.SetSameLineFlag( false );
    m_DomeEditLayout.SetFitWidthFlag( true );
    m_DomeEditLayout.ForceNewLine();
    m_DomeEditLayout.SetChoiceButtonWidth( m_DomeEditLayout.GetRemainX() / 3 );

    m_DomeEditLayout.AddChoice( m_DomePropertyChoice, "Property" );
    m_DomeEditLayout.AddChoice( m_DomeCapPropertyChoice, "Cap Property" );

    //==== FeaRibArray ====//
    m_GenLayout.AddSubGroupLayout( m_RibArrayEditLayout, m_GenLayout.GetW(), m_GenLayout.GetH() );
    m_RibArrayEditLayout.SetY( start_y );

    m_RibArrayEditLayout.AddDividerBox( "RibArray" );

    m_RibArrayEditLayout.AddChoice( m_RibArrayPerpEdgeChoice, "Edge Normal" );

    m_RibArrayEditLayout.AddYGap();

    m_RibArrayPosTypeChoice.AddItem( "% Span" );
    m_RibArrayPosTypeChoice.AddItem( "Dist Span" );
    m_RibArrayEditLayout.AddChoice( m_RibArrayPosTypeChoice, "Parameterization" );

    m_RibArrayEditLayout.SetButtonWidth( m_RibArrayEditLayout.GetRemainX() / 3 );

    m_RibArrayEditLayout.SetSameLineFlag( true );
    m_RibArrayEditLayout.SetFitWidthFlag( false );

    m_RibArrayEditLayout.AddLabel( "Direction", m_RibArrayEditLayout.GetRemainX() / 3 );
    m_RibArrayEditLayout.AddButton( m_RibArrayNegDirToggle, "Negative" );
    m_RibArrayEditLayout.AddButton( m_RibArrayPosDirToggle, "Positive" );

    m_RibArrayPosNegDirToggleGroup.Init( this );
    m_RibArrayPosNegDirToggleGroup.AddButton( m_RibArrayNegDirToggle.GetFlButton() );
    m_RibArrayPosNegDirToggleGroup.AddButton( m_RibArrayPosDirToggle.GetFlButton() );

    m_RibArrayEditLayout.ForceNewLine();

    button_width = m_RibArrayEditLayout.GetButtonWidth();
    slider_width = m_RibArrayEditLayout.GetSliderWidth();

    m_RibArrayEditLayout.SetSliderWidth( slider_width - 5 );

    m_RibArrayEditLayout.AddSlider( m_RibArrayStartLocSlider, "Start Location", 50, "%5.3f" );

    m_RibArrayEditLayout.AddButton( m_RibArrayStartLocUnit, " " );
    m_RibArrayStartLocUnit.GetFlButton()->box( FL_THIN_UP_BOX );
    m_RibArrayStartLocUnit.GetFlButton()->labelcolor( FL_BLACK );
    m_RibArrayStartLocUnit.SetWidth( m_RibArrayEditLayout.GetW() / 14 );

    m_RibArrayEditLayout.ForceNewLine();

    m_RibArrayEditLayout.AddSlider( m_RibArraySpacingSlider, "Spacing", 50, "%5.3f" );

    m_RibArrayEditLayout.AddButton( m_RibArrayPosUnit, " " );
    m_RibArrayPosUnit.GetFlButton()->box( FL_THIN_UP_BOX );
    m_RibArrayPosUnit.GetFlButton()->labelcolor( FL_BLACK );
    m_RibArrayPosUnit.SetWidth( m_RibArrayEditLayout.GetW() / 14 );

    m_RibArrayEditLayout.ForceNewLine();
    m_RibArrayEditLayout.AddYGap();

    m_RibArrayEditLayout.InitWidthHeightVals();
    m_RibArrayEditLayout.SetSameLineFlag( false );
    m_RibArrayEditLayout.SetFitWidthFlag( true );

    m_RibArrayEditLayout.SetButtonWidth( button_width );

    m_RibArrayEditLayout.AddSlider( m_RibArrayThetaSlider, "Rotation", 25, "%5.3f" );

    m_RibArrayEditLayout.AddYGap();

    m_RibArrayEditLayout.SetSameLineFlag( true );
    m_RibArrayEditLayout.SetFitWidthFlag( false );

    m_RibArrayEditLayout.SetButtonWidth( m_RibArrayEditLayout.GetRemainX() / 3 );
    m_RibArrayEditLayout.AddButton( m_RibArrayShellToggle, "Shell" );
    m_RibArrayEditLayout.AddButton( m_RibArrayCapToggle, "Cap" );
    m_RibArrayEditLayout.AddButton( m_RibArrayShellCapToggle, "Shell and Cap" );

    m_RibArrayShellCapToggleGroup.Init( this );
    m_RibArrayShellCapToggleGroup.AddButton( m_RibArrayShellToggle.GetFlButton() );
    m_RibArrayShellCapToggleGroup.AddButton( m_RibArrayCapToggle.GetFlButton() );
    m_RibArrayShellCapToggleGroup.AddButton( m_RibArrayShellCapToggle.GetFlButton() );

    m_RibArrayEditLayout.SetSameLineFlag( false );
    m_RibArrayEditLayout.SetFitWidthFlag( true );
    m_RibArrayEditLayout.ForceNewLine();
    m_RibArrayEditLayout.SetChoiceButtonWidth( m_RibArrayEditLayout.GetRemainX() / 3 );

    m_RibArrayEditLayout.AddChoice( m_RibArrayPropertyChoice, "Property" );
    m_RibArrayEditLayout.AddChoice( m_RibArrayCapPropertyChoice, "Cap Property" );

    m_RibArrayEditLayout.AddYGap();

    m_RibArrayEditLayout.AddButton( m_IndividualizeRibArrayButton, "Individualize Rib Array" );

    //==== FeaStiffenerArray ====//
    m_GenLayout.AddSubGroupLayout( m_StiffenerArrayEditLayout, m_GenLayout.GetW(), m_GenLayout.GetH() );
    m_StiffenerArrayEditLayout.SetY( start_y );

    m_StiffenerArrayEditLayout.AddDividerBox( "StiffenerArray" );

    m_StiffenerArrayEditLayout.SetChoiceButtonWidth( m_StiffenerArrayEditLayout.GetRemainX() / 3 );

    m_StiffenerArrayPosTypeChoice.AddItem( "% Spine" );
    m_StiffenerArrayPosTypeChoice.AddItem( "Dist Spine" );
    m_StiffenerArrayEditLayout.AddChoice( m_StiffenerArrayPosTypeChoice, "Parameterization" );

    m_StiffenerArrayEditLayout.SetButtonWidth( m_StiffenerArrayEditLayout.GetRemainX() / 3 );

    m_StiffenerArrayEditLayout.SetSameLineFlag( true );
    m_StiffenerArrayEditLayout.SetFitWidthFlag( false );

    m_StiffenerArrayEditLayout.AddLabel( "Direction", m_StiffenerArrayEditLayout.GetRemainX() / 3 );
    m_StiffenerArrayEditLayout.AddButton( m_StiffenerArrayNegDirToggle, "Negative" );
    m_StiffenerArrayEditLayout.AddButton( m_StiffenerArrayPosDirToggle, "Positive" );

    m_StiffenerArrayPosNegDirToggleGroup.Init( this );
    m_StiffenerArrayPosNegDirToggleGroup.AddButton( m_StiffenerArrayNegDirToggle.GetFlButton() );
    m_StiffenerArrayPosNegDirToggleGroup.AddButton( m_StiffenerArrayPosDirToggle.GetFlButton() );

    m_StiffenerArrayEditLayout.ForceNewLine();

    button_width = m_StiffenerArrayEditLayout.GetButtonWidth();
    slider_width = m_StiffenerArrayEditLayout.GetSliderWidth();

    m_StiffenerArrayEditLayout.SetSliderWidth( slider_width - 5 );

    m_StiffenerArrayEditLayout.AddSlider( m_StiffenerArrayStartLocSlider, "Start Location", 50, "%5.3f" );

    m_StiffenerArrayEditLayout.AddButton( m_StiffenerArrayStartLocUnit, " " );
    m_StiffenerArrayStartLocUnit.GetFlButton()->box( FL_THIN_UP_BOX );
    m_StiffenerArrayStartLocUnit.GetFlButton()->labelcolor( FL_BLACK );
    m_StiffenerArrayStartLocUnit.SetWidth( m_StiffenerArrayEditLayout.GetW() / 14 );

    m_StiffenerArrayEditLayout.ForceNewLine();

    m_StiffenerArrayEditLayout.AddSlider( m_StiffenerArraySpacingSlider, "Spacing", 50, "%5.3f" );

    m_StiffenerArrayEditLayout.AddButton( m_StiffenerArrayPosUnit, " " );
    m_StiffenerArrayPosUnit.GetFlButton()->box( FL_THIN_UP_BOX );
    m_StiffenerArrayPosUnit.GetFlButton()->labelcolor( FL_BLACK );
    m_StiffenerArrayPosUnit.SetWidth( m_StiffenerArrayEditLayout.GetW() / 14 );

    m_StiffenerArrayEditLayout.ForceNewLine();
    m_StiffenerArrayEditLayout.AddYGap();

    m_StiffenerArrayEditLayout.SetSameLineFlag( false );
    m_StiffenerArrayEditLayout.SetFitWidthFlag( true );

    m_StiffenerArrayEditLayout.AddChoice( m_StiffenerArrayCapPropertyChoice, "Cap Property" );

    m_StiffenerArrayEditLayout.AddYGap();

    m_StiffenerArrayEditLayout.AddButton( m_IndividualizeStiffenerArrayButton, "Individualize Stiffener Array" );

    //=== SubSurfaces ===//

    //==== SSLine ====//
    m_GenLayout.AddSubGroupLayout( m_FeaSSLineGroup, m_GenLayout.GetW(), m_GenLayout.GetH() );
    m_FeaSSLineGroup.SetY( start_y );

    m_FeaSSLineGroup.AddDividerBox( "Line Sub-Surface" );

    m_FeaSSLineGroup.SetFitWidthFlag( false );
    m_FeaSSLineGroup.SetSameLineFlag( true );
    m_FeaSSLineGroup.AddLabel( "Tag", m_FeaSSLineGroup.GetRemainX() / 3 );
    m_FeaSSLineGroup.SetButtonWidth( m_FeaSSLineGroup.GetRemainX() / 3 );
    m_FeaSSLineGroup.AddButton( m_FeaSSLineGreaterToggle, "Greater" );
    m_FeaSSLineGroup.AddButton( m_FeaSSLineLessToggle, "Less" );
    m_FeaSSLineGroup.AddButton( m_FeaSSLineNoneToggle, "None" );

    m_FeaSSLineTestToggleGroup.Init( this );
    m_FeaSSLineTestToggleGroup.AddButton( m_FeaSSLineGreaterToggle.GetFlButton() );
    m_FeaSSLineTestToggleGroup.AddButton( m_FeaSSLineLessToggle.GetFlButton() );
    m_FeaSSLineTestToggleGroup.AddButton( m_FeaSSLineNoneToggle.GetFlButton() );

    m_FeaSSLineGroup.ForceNewLine();

    m_FeaSSLineGroup.AddLabel( "Line Type", m_FeaSSLineGroup.GetRemainX() / 3 );
    m_FeaSSLineGroup.SetButtonWidth( m_FeaSSLineGroup.GetRemainX() / 2 );
    m_FeaSSLineGroup.AddButton( m_FeaSSLineConstUButton, "U" );
    m_FeaSSLineGroup.AddButton( m_FeaSSLineConstWButton, "W" );

    m_FeaSSLineConstToggleGroup.Init( this );
    m_FeaSSLineConstToggleGroup.AddButton( m_FeaSSLineConstUButton.GetFlButton() );
    m_FeaSSLineConstToggleGroup.AddButton( m_FeaSSLineConstWButton.GetFlButton() );

    m_FeaSSLineGroup.ForceNewLine();
    m_FeaSSLineGroup.SetFitWidthFlag( true );
    m_FeaSSLineGroup.SetSameLineFlag( false );

    m_FeaSSLineGroup.SetButtonWidth( m_FeaSSLineGroup.GetRemainX() / 3 );
    m_FeaSSLineGroup.SetChoiceButtonWidth( m_FeaSSLineGroup.GetRemainX() / 3 );

    m_FeaSSLineGroup.SetButtonWidth( m_FeaSSLineGroup.GetRemainX() / 3 );
    m_FeaSSLineGroup.AddSlider( m_FeaSSLineConstSlider, "Value", 1, "%5.4f" );

    m_FeaSSLineGroup.AddYGap();

    m_FeaSSLineGroup.SetSameLineFlag( true );
    m_FeaSSLineGroup.SetFitWidthFlag( false );

    m_FeaSSLineGroup.SetButtonWidth( m_FeaSSLineGroup.GetRemainX() / 3 );
    m_FeaSSLineGroup.AddButton( m_FeaSSLineShellToggle, "Shell" );
    m_FeaSSLineGroup.AddButton( m_FeaSSLineCapToggle, "Cap (no tris)" );
    m_FeaSSLineGroup.AddButton( m_FeaSSLineShellCapToggle, "Shell and Cap" );

    m_FeaSSLineShellCapToggleGroup.Init( this );
    m_FeaSSLineShellCapToggleGroup.AddButton( m_FeaSSLineShellToggle.GetFlButton() );
    m_FeaSSLineShellCapToggleGroup.AddButton( m_FeaSSLineCapToggle.GetFlButton() );
    m_FeaSSLineShellCapToggleGroup.AddButton( m_FeaSSLineShellCapToggle.GetFlButton() );

    m_FeaSSLineGroup.SetSameLineFlag( false );
    m_FeaSSLineGroup.SetFitWidthFlag( true );
    m_FeaSSLineGroup.ForceNewLine();

    m_FeaSSLineGroup.AddChoice( m_FeaSSLinePropertyChoice, "Property" );
    m_FeaSSLineGroup.AddChoice( m_FeaSSLineCapPropertyChoice, "Cap Property" );

    //==== SSRectangle ====//
    m_GenLayout.AddSubGroupLayout( m_FeaSSRecGroup, m_GenLayout.GetW(), m_GenLayout.GetH() );
    m_FeaSSRecGroup.SetY( start_y );

    m_FeaSSRecGroup.AddDividerBox( "Rectangle Sub-Surface" );

    m_FeaSSRecGroup.SetFitWidthFlag( false );
    m_FeaSSRecGroup.SetSameLineFlag( true );
    m_FeaSSRecGroup.AddLabel( "Tag", m_FeaSSRecGroup.GetRemainX() / 3 );
    m_FeaSSRecGroup.SetButtonWidth( m_FeaSSRecGroup.GetRemainX() / 3 );
    m_FeaSSRecGroup.AddButton( m_FeaSSRecInsideButton, "Inside" );
    m_FeaSSRecGroup.AddButton( m_FeaSSRecOutsideButton, "Outside" );
    m_FeaSSRecGroup.AddButton( m_FeaSSRecNoneButton, "None" );

    m_FeaSSRecTestToggleGroup.Init( this );
    m_FeaSSRecTestToggleGroup.AddButton( m_FeaSSRecInsideButton.GetFlButton() );
    m_FeaSSRecTestToggleGroup.AddButton( m_FeaSSRecOutsideButton.GetFlButton() );
    m_FeaSSRecTestToggleGroup.AddButton( m_FeaSSRecNoneButton.GetFlButton() );

    m_FeaSSRecGroup.ForceNewLine();
    m_FeaSSRecGroup.SetFitWidthFlag( true );
    m_FeaSSRecGroup.SetSameLineFlag( false );

    m_FeaSSRecGroup.SetButtonWidth( m_FeaSSRecGroup.GetRemainX() / 3 );
    m_FeaSSRecGroup.SetChoiceButtonWidth( m_FeaSSRecGroup.GetRemainX() / 3 );

    m_FeaSSRecGroup.AddSlider( m_FeaSSRecCentUSlider, "Center U", 1, "%5.4f" );
    m_FeaSSRecGroup.AddSlider( m_FeaSSRecCentWSlider, "Center W", 1, "%5.4f" );
    m_FeaSSRecGroup.AddSlider( m_FeaSSRecULenSlider, "U Length", 1, "%5.4f" );
    m_FeaSSRecGroup.AddSlider( m_FeaSSRecWLenSlider, "W Length", 1, "%5.4f" );
    m_FeaSSRecGroup.AddSlider( m_FeaSSRecThetaSlider, "Theta", 25, "%5.4f" );

    m_FeaSSRecGroup.AddYGap();

    m_FeaSSRecGroup.SetSameLineFlag( true );
    m_FeaSSRecGroup.SetFitWidthFlag( false );

    m_FeaSSRecGroup.SetButtonWidth( m_FeaSSRecGroup.GetRemainX() / 3 );
    m_FeaSSRecGroup.AddButton( m_FeaSSRecShellToggle, "Shell" );
    m_FeaSSRecGroup.AddButton( m_FeaSSRecCapToggle, "Cap (no tris)" );
    m_FeaSSRecGroup.AddButton( m_FeaSSRecShellCapToggle, "Shell and Cap" );

    m_FeaSSRecShellCapToggleGroup.Init( this );
    m_FeaSSRecShellCapToggleGroup.AddButton( m_FeaSSRecShellToggle.GetFlButton() );
    m_FeaSSRecShellCapToggleGroup.AddButton( m_FeaSSRecCapToggle.GetFlButton() );
    m_FeaSSRecShellCapToggleGroup.AddButton( m_FeaSSRecShellCapToggle.GetFlButton() );

    m_FeaSSRecGroup.SetSameLineFlag( false );
    m_FeaSSRecGroup.SetFitWidthFlag( true );
    m_FeaSSRecGroup.ForceNewLine();

    m_FeaSSRecGroup.AddChoice( m_FeaSSRecPropertyChoice, "Property" );
    m_FeaSSRecGroup.AddChoice( m_FeaSSRecCapPropertyChoice, "Cap Property" );

    //==== SS_Ellipse ====//
    m_GenLayout.AddSubGroupLayout( m_FeaSSEllGroup, m_GenLayout.GetW(), m_GenLayout.GetH() );
    m_FeaSSEllGroup.SetY( start_y );

    m_FeaSSEllGroup.AddDividerBox( "Ellipse Sub-Surface" );

    m_FeaSSEllGroup.SetFitWidthFlag( false );
    m_FeaSSEllGroup.SetSameLineFlag( true );
    m_FeaSSEllGroup.AddLabel( "Tag", m_FeaSSEllGroup.GetRemainX() / 3 );
    m_FeaSSEllGroup.SetButtonWidth( m_FeaSSEllGroup.GetRemainX() / 3 );
    m_FeaSSEllGroup.AddButton( m_FeaSSEllInsideButton, "Inside" );
    m_FeaSSEllGroup.AddButton( m_FeaSSEllOutsideButton, "Outside" );
    m_FeaSSEllGroup.AddButton( m_FeaSSEllNoneButton, "None" );

    m_FeaSSEllTestToggleGroup.Init( this );
    m_FeaSSEllTestToggleGroup.AddButton( m_FeaSSEllInsideButton.GetFlButton() );
    m_FeaSSEllTestToggleGroup.AddButton( m_FeaSSEllOutsideButton.GetFlButton() );
    m_FeaSSEllTestToggleGroup.AddButton( m_FeaSSEllNoneButton.GetFlButton() );

    m_FeaSSEllGroup.ForceNewLine();
    m_FeaSSEllGroup.SetFitWidthFlag( true );
    m_FeaSSEllGroup.SetSameLineFlag( false );

    m_FeaSSEllGroup.SetButtonWidth( m_FeaSSEllGroup.GetRemainX() / 3 );
    m_FeaSSEllGroup.SetChoiceButtonWidth( m_FeaSSEllGroup.GetRemainX() / 3 );

    m_FeaSSEllGroup.AddSlider( m_FeaSSEllTessSlider, "Num Points", 100, "%5.0f" );
    m_FeaSSEllGroup.AddSlider( m_FeaSSEllCentUSlider, "Center U", 1, "%5.4f" );
    m_FeaSSEllGroup.AddSlider( m_FeaSSEllCentWSlider, "Center W", 1, "%5.4f" );
    m_FeaSSEllGroup.AddSlider( m_FeaSSEllULenSlider, "U Length", 1, "%5.4f" );
    m_FeaSSEllGroup.AddSlider( m_FeaSSEllWLenSlider, "W Length", 1, "%5.4f" );
    m_FeaSSEllGroup.AddSlider( m_FeaSSEllThetaSlider, "Theta", 25, "%5.4f" );

    m_FeaSSEllGroup.AddYGap();

    m_FeaSSEllGroup.SetSameLineFlag( true );
    m_FeaSSEllGroup.SetFitWidthFlag( false );

    m_FeaSSEllGroup.SetButtonWidth( m_FeaSSEllGroup.GetRemainX() / 3 );
    m_FeaSSEllGroup.AddButton( m_FeaSSEllShellToggle, "Shell" );
    m_FeaSSEllGroup.AddButton( m_FeaSSEllCapToggle, "Cap (no tris)" );
    m_FeaSSEllGroup.AddButton( m_FeaSSEllShellCapToggle, "Shell and Cap" );

    m_FeaSSEllShellCapToggleGroup.Init( this );
    m_FeaSSEllShellCapToggleGroup.AddButton( m_FeaSSEllShellToggle.GetFlButton() );
    m_FeaSSEllShellCapToggleGroup.AddButton( m_FeaSSEllCapToggle.GetFlButton() );
    m_FeaSSEllShellCapToggleGroup.AddButton( m_FeaSSEllShellCapToggle.GetFlButton() );

    m_FeaSSEllGroup.SetSameLineFlag( false );
    m_FeaSSEllGroup.SetFitWidthFlag( true );
    m_FeaSSEllGroup.ForceNewLine();

    m_FeaSSEllGroup.AddChoice( m_FeaSSEllPropertyChoice, "Property" );
    m_FeaSSEllGroup.AddChoice( m_FeaSSEllCapPropertyChoice, "Cap Property" );

    //===== SSControl ====//
    m_GenLayout.AddSubGroupLayout( m_FeaSSConGroup, m_GenLayout.GetW(), m_GenLayout.GetH() );
    m_FeaSSConGroup.SetY( start_y );

    m_FeaSSConGroup.AddDividerBox( "Control Sub-Surface" );

    m_FeaSSConGroup.SetFitWidthFlag( false );
    m_FeaSSConGroup.SetSameLineFlag( true );
    m_FeaSSConGroup.AddLabel( "Tag", m_FeaSSConGroup.GetRemainX() / 3 );
    m_FeaSSConGroup.SetButtonWidth( m_FeaSSConGroup.GetRemainX() / 3 );
    m_FeaSSConGroup.AddButton( m_FeaSSConInsideButton, "Inside" );
    m_FeaSSConGroup.AddButton( m_FeaSSConOutsideButton, "Outside" );
    m_FeaSSConGroup.AddButton( m_FeaSSConNoneButton, "None" );

    m_FeaSSConTestToggleGroup.Init( this );
    m_FeaSSConTestToggleGroup.AddButton( m_FeaSSConInsideButton.GetFlButton() );
    m_FeaSSConTestToggleGroup.AddButton( m_FeaSSConOutsideButton.GetFlButton() );
    m_FeaSSConTestToggleGroup.AddButton( m_FeaSSConNoneButton.GetFlButton() );

    m_FeaSSConGroup.ForceNewLine();

    m_FeaSSConGroup.SetChoiceButtonWidth( m_FeaSSConGroup.GetRemainX() / 3 );

    m_FeaSSConSurfTypeChoice.AddItem( "Upper" );
    m_FeaSSConSurfTypeChoice.AddItem( "Lower" );
    m_FeaSSConSurfTypeChoice.AddItem( "Both" );
    m_FeaSSConGroup.AddChoice( m_FeaSSConSurfTypeChoice, "Upper/Lower", m_FeaSSConGroup.GetRemainX() / 3 );

    m_FeaSSConGroup.SetButtonWidth( m_FeaSSConGroup.GetRemainX() );

    m_FeaSSConGroup.AddButton( m_FeaSSConLEFlagButton, "Leading Edge" );

    m_FeaSSConGroup.SetFitWidthFlag( true );
    m_FeaSSConGroup.SetSameLineFlag( false );
    m_FeaSSConGroup.ForceNewLine();

    m_FeaSSConGroup.AddYGap();
    m_FeaSSConGroup.AddDividerBox( "Spanwise" );

    m_FeaSSConGroup.SetButtonWidth( m_FeaSSConGroup.GetRemainX() / 3 );

    m_FeaSSConGroup.AddSlider( m_FeaSSConUSSlider, "Start U", 1, "%5.4f" );
    m_FeaSSConGroup.AddSlider( m_FeaSSConUESlider, "End U", 1, "%5.4f" );

    m_FeaSSConGroup.AddYGap();
    m_FeaSSConGroup.AddDividerBox( "Chordwise" );

    m_FeaSSConGroup.SetFitWidthFlag( false );
    m_FeaSSConGroup.SetSameLineFlag( true );

    m_FeaSSConGroup.SetButtonWidth( m_FeaSSConGroup.GetW() / 3 );

    m_FeaSSConGroup.AddButton( m_FeaSSConSAbsButton, "Length" );
    m_FeaSSConGroup.AddButton( m_FeaSSConSRelButton, "Length/C" );
    m_FeaSSConGroup.AddButton( m_FeaSSConSEConstButton, "Constant" );

    m_FeaSSConGroup.SetFitWidthFlag( true );
    m_FeaSSConGroup.SetSameLineFlag( false );
    m_FeaSSConGroup.ForceNewLine();

    m_FeaSSConSAbsRelToggleGroup.Init( this );
    m_FeaSSConSAbsRelToggleGroup.AddButton( m_FeaSSConSAbsButton.GetFlButton() );
    m_FeaSSConSAbsRelToggleGroup.AddButton( m_FeaSSConSRelButton.GetFlButton() );

    m_FeaSSConGroup.SetButtonWidth( m_FeaSSConGroup.GetRemainX() / 3 );

    m_FeaSSConGroup.AddSlider( m_FeaSSConSLenSlider, "Start Length", 10.0, "%5.4f" );
    m_FeaSSConGroup.AddSlider( m_FeaSSConSFracSlider, "Start Length/C", 1.0, "%5.4f" );
    m_FeaSSConGroup.AddSlider( m_FeaSSConELenSlider, "End Length", 10.0, "%5.4f" );
    m_FeaSSConGroup.AddSlider( m_FeaSSConEFracSlider, "End Length/C", 1.0, "%5.4f" );

    m_FeaSSConGroup.AddYGap();
    m_FeaSSConGroup.AddDividerBox( "Surface End Angle" );

    m_FeaSSConGroup.SetSameLineFlag( true );
    m_FeaSSConGroup.SetFitWidthFlag( false );

    m_FeaSSConGroup.AddButton( m_FeaSSConSAngleButton, "Start" );
    m_FeaSSConGroup.AddButton( m_FeaSSConEAngleButton, "End" );
    m_FeaSSConGroup.AddButton( m_FeaSSConSameAngleButton, "Same Angle" );

    m_FeaSSConGroup.SetSameLineFlag( false );
    m_FeaSSConGroup.SetFitWidthFlag( true );
    m_FeaSSConGroup.ForceNewLine();

    m_FeaSSConGroup.AddSlider( m_FeaSSConSAngleSlider, "Start Angle", 10.0, "%5.4f" );
    m_FeaSSConGroup.AddSlider( m_FeaSSConEAngleSlider, "End Angle", 10.0, "%5.4f" );

    m_FeaSSConGroup.AddSlider( m_FeaSSConTessSlider, "Num Points", 100, "%5.0f" );

    m_FeaSSConGroup.AddYGap();

    m_FeaSSConGroup.SetSameLineFlag( true );
    m_FeaSSConGroup.SetFitWidthFlag( false );

    m_FeaSSConGroup.SetButtonWidth( m_FeaSSConGroup.GetRemainX() / 3 );
    m_FeaSSConGroup.AddButton( m_FeaSSConShellToggle, "Shell" );
    m_FeaSSConGroup.AddButton( m_FeaSSConCapToggle, "Cap (no tris)" );
    m_FeaSSConGroup.AddButton( m_FeaSSConShellCapToggle, "Shell and Cap" );

    m_FeaSSConShellCapToggleGroup.Init( this );
    m_FeaSSConShellCapToggleGroup.AddButton( m_FeaSSConShellToggle.GetFlButton() );
    m_FeaSSConShellCapToggleGroup.AddButton( m_FeaSSConCapToggle.GetFlButton() );
    m_FeaSSConShellCapToggleGroup.AddButton( m_FeaSSConShellCapToggle.GetFlButton() );

    m_FeaSSConGroup.SetSameLineFlag( false );
    m_FeaSSConGroup.SetFitWidthFlag( true );
    m_FeaSSConGroup.ForceNewLine();

    m_FeaSSConGroup.AddChoice( m_FeaSSConPropertyChoice, "Property" );
    m_FeaSSConGroup.AddChoice( m_FeaSSConCapPropertyChoice, "Cap Property" );

    //==== SSLineArray ====//
    m_GenLayout.AddSubGroupLayout( m_FeaSSLineArrayGroup, m_GenLayout.GetW(), m_GenLayout.GetH() );
    m_FeaSSLineArrayGroup.SetY( start_y );

    m_FeaSSLineArrayGroup.AddDividerBox( "Line Array Sub-Surface" );

    m_FeaSSLineArrayGroup.SetSameLineFlag( true );
    m_FeaSSLineArrayGroup.SetFitWidthFlag( false );

    m_FeaSSLineArrayGroup.AddLabel( "Line Type", m_FeaSSLineArrayGroup.GetRemainX() / 3 );
    m_FeaSSLineArrayGroup.SetButtonWidth( m_FeaSSLineArrayGroup.GetRemainX() / 2 );
    m_FeaSSLineArrayGroup.AddButton( m_FeaSSLineArrayConstUButton, "U" );
    m_FeaSSLineArrayGroup.AddButton( m_FeaSSLineArrayConstWButton, "W" );

    m_FeaSSLineArrayConstToggleGroup.Init( this );
    m_FeaSSLineArrayConstToggleGroup.AddButton( m_FeaSSLineArrayConstUButton.GetFlButton() );
    m_FeaSSLineArrayConstToggleGroup.AddButton( m_FeaSSLineArrayConstWButton.GetFlButton() );

    m_FeaSSLineArrayGroup.ForceNewLine();

    m_FeaSSLineArrayGroup.SetButtonWidth( m_FeaSSLineArrayGroup.GetRemainX() / 3 );

    m_FeaSSLineArrayGroup.AddLabel( "Direction", m_FeaSSLineArrayGroup.GetRemainX() / 3 );
    m_FeaSSLineArrayGroup.AddButton( m_FeaSSLineArrayNegDirToggle, "Negative" );
    m_FeaSSLineArrayGroup.AddButton( m_FeaSSLineArrayPosDirToggle, "Positive" );

    m_FeaSSLineArrayPosNegDirToggleGroup.Init( this );
    m_FeaSSLineArrayPosNegDirToggleGroup.AddButton( m_FeaSSLineArrayNegDirToggle.GetFlButton() );
    m_FeaSSLineArrayPosNegDirToggleGroup.AddButton( m_FeaSSLineArrayPosDirToggle.GetFlButton() );

    m_FeaSSLineArrayGroup.ForceNewLine();
    m_FeaSSLineArrayGroup.SetSameLineFlag( false );
    m_FeaSSLineArrayGroup.SetFitWidthFlag( true );

    m_FeaSSLineArrayGroup.AddSlider( m_FeaSSLineArraySpacingSlider, "Spacing", 1, "%5.4f" );
    m_FeaSSLineArrayGroup.AddSlider( m_FeaSSLineArrayStartLocSlider, "Start Location", 1, "%5.4f" );

    m_FeaSSLineArrayGroup.AddYGap();

    m_FeaSSLineArrayGroup.AddChoice( m_FeaSSLineArrayCapPropertyChoice, "Cap Property" );
}

FeaPartEditScreen::~FeaPartEditScreen()
{
}

void FeaPartEditScreen::Show()
{
    if ( Update() )
    {
        m_FLTK_Window->show();
    }
}

bool FeaPartEditScreen::Update()
{
    assert( m_ScreenMgr );

    // Close the screen if StructScreen is not open
    VspScreen* struct_screen = m_ScreenMgr->GetScreen( ScreenMgr::VSP_STRUCT_SCREEN );
    if ( !struct_screen )
    {
        Hide();
        return false;
    }
    else if ( !struct_screen->IsShown() )
    {
        Hide();
        return false;
    }

    Vehicle* veh = m_ScreenMgr->GetVehiclePtr();

    if ( veh )
    {
        //==== Update Perpendicular Rib Choice ====//
        UpdatePerpendicularRibChoice();

        //==== Update FixPoint Parent Surf Choice ====//
        UpdateFixPointParentChoice();

        //===== FeaProperty Update =====//
        UpdateFeaPropertyChoice();

        UpdateUnitLabels();

        if ( StructureMgr.ValidTotalFeaStructInd( StructureMgr.GetCurrStructIndex() ) )
        {
            vector< FeaStructure* > structVec = StructureMgr.GetAllFeaStructs();

            // Update Current FeaPart
            if ( StructureMgr.GetCurrPartIndex() < structVec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() )
            {
                FeaPart* feaprt = structVec[StructureMgr.GetCurrStructIndex()]->GetFeaPart( StructureMgr.GetCurrPartIndex() );
                if ( feaprt )
                {
                    m_FeaPartNameInput.Update( feaprt->GetName() );

                    if ( feaprt->GetType() == vsp::FEA_SKIN )
                    {
                        FeaSkin* skin = dynamic_cast<FeaSkin*>( feaprt );
                        assert( skin );

                        m_RemoveSkinTrisToggle.Update( skin->m_RemoveSkinTrisFlag.GetID() );

                        if ( skin->m_RemoveSkinTrisFlag() )
                        {
                            m_SkinPropertyChoice.Deactivate();
                        }
                        else
                        {
                            m_SkinPropertyChoice.Activate();
                        }

                        FeaPartDispGroup( &m_SkinEditLayout );
                    }
                    else if ( feaprt->GetType() == vsp::FEA_SLICE )
                    {
                        FeaSlice* slice = dynamic_cast<FeaSlice*>( feaprt );
                        assert( slice );

                        m_SliceOrientationChoice.Update( slice->m_OrientationPlane.GetID() );
                        m_SlicePosTypeChoice.Update( slice->m_LocationParmType.GetID() );
                        m_SliceCenterLocSlider.Update( slice->m_CenterLocation.GetID() );
                        m_SliceRotAxisToggleGroup.Update( slice->m_RotationAxis.GetID() );
                        m_SliceXRotSlider.Update( slice->m_XRot.GetID() );
                        m_SliceYRotSlider.Update( slice->m_YRot.GetID() );
                        m_SliceZRotSlider.Update( slice->m_ZRot.GetID() );
                        m_SliceShellCapToggleGroup.Update( slice->m_IncludedElements.GetID() );

                        if ( slice->m_RotationAxis() == vsp::X_DIR )
                        {
                            m_SliceXRotSlider.Activate();
                            m_SliceYRotSlider.Deactivate();
                            m_SliceZRotSlider.Deactivate();
                            slice->m_YRot.Set( 0.0 );
                            slice->m_ZRot.Set( 0.0 );
                        }
                        else if ( slice->m_RotationAxis() == vsp::Y_DIR )
                        {
                            m_SliceXRotSlider.Deactivate();
                            m_SliceYRotSlider.Activate();
                            m_SliceZRotSlider.Deactivate();
                            slice->m_XRot.Set( 0.0 );
                            slice->m_ZRot.Set( 0.0 );
                        }
                        else if ( slice->m_RotationAxis() == vsp::Z_DIR )
                        {
                            m_SliceXRotSlider.Deactivate();
                            m_SliceYRotSlider.Deactivate();
                            m_SliceZRotSlider.Activate();
                            slice->m_XRot.Set( 0.0 );
                            slice->m_YRot.Set( 0.0 );
                        }

                        if ( slice->m_IncludedElements() == vsp::FEA_SHELL_AND_BEAM )
                        {
                            m_SlicePropertyChoice.Activate();
                            m_SliceCapPropertyChoice.Activate();
                        }
                        else if ( slice->m_IncludedElements() == vsp::FEA_SHELL )
                        {
                            m_SlicePropertyChoice.Activate();
                            m_SliceCapPropertyChoice.Deactivate();
                        }
                        else if ( slice->m_IncludedElements() == vsp::FEA_BEAM )
                        {
                            m_SlicePropertyChoice.Deactivate();
                            m_SliceCapPropertyChoice.Activate();
                        }

                        FeaPartDispGroup( &m_SliceEditLayout );
                    }
                    else if ( feaprt->GetType() == vsp::FEA_RIB )
                    {
                        FeaRib* rib = dynamic_cast<FeaRib*>( feaprt );
                        assert( rib );

                        m_RibPosTypeChoice.Update( rib->m_LocationParmType.GetID() );
                        m_RibPosSlider.Update( rib->m_CenterLocation.GetID() );
                        m_RibThetaSlider.Update( rib->m_Theta.GetID() );
                        m_RibShellCapToggleGroup.Update( rib->m_IncludedElements.GetID() );

                        if ( rib->m_IncludedElements() == vsp::FEA_SHELL_AND_BEAM )
                        {
                            m_RibPropertyChoice.Activate();
                            m_RibCapPropertyChoice.Activate();
                        }
                        else if ( rib->m_IncludedElements() == vsp::FEA_SHELL )
                        {
                            m_RibPropertyChoice.Activate();
                            m_RibCapPropertyChoice.Deactivate();
                        }
                        else if ( rib->m_IncludedElements() == vsp::FEA_BEAM )
                        {
                            m_RibPropertyChoice.Deactivate();
                            m_RibCapPropertyChoice.Activate();
                        }

                        FeaPartDispGroup( &m_RibEditLayout );
                    }
                    else if ( feaprt->GetType() == vsp::FEA_SPAR )
                    {
                        FeaSpar* spar = dynamic_cast<FeaSpar*>( feaprt );
                        assert( spar );

                        m_SparPosTypeChoice.Update( spar->m_LocationParmType.GetID() );
                        m_SparSectionLimitToggle.Update( spar->m_LimitSparToSectionFlag.GetID() );
                        m_SparSectIndexSelector.Update( spar->m_CurrWingSection.GetID() );

                        if ( spar->m_LimitSparToSectionFlag() )
                        {
                            m_SparSectIndexSelector.Activate();
                        }
                        else
                        {
                            m_SparSectIndexSelector.Deactivate();
                        }

                        m_SparPosSlider.Update( spar->m_CenterLocation.GetID() );
                        m_SparThetaSlider.Update( spar->m_Theta.GetID() );
                        m_SparShellCapToggleGroup.Update( spar->m_IncludedElements.GetID() );

                        if ( spar->m_IncludedElements() == vsp::FEA_SHELL_AND_BEAM )
                        {
                            m_SparPropertyChoice.Activate();
                            m_SparCapPropertyChoice.Activate();
                        }
                        else if ( spar->m_IncludedElements() == vsp::FEA_SHELL )
                        {
                            m_SparPropertyChoice.Activate();
                            m_SparCapPropertyChoice.Deactivate();
                        }
                        else if ( spar->m_IncludedElements() == vsp::FEA_BEAM )
                        {
                            m_SparPropertyChoice.Deactivate();
                            m_SparCapPropertyChoice.Activate();
                        }

                        FeaPartDispGroup( &m_SparEditLayout );
                    }
                    else if ( feaprt->GetType() == vsp::FEA_FIX_POINT )
                    {
                        FeaFixPoint* fixpt = dynamic_cast<FeaFixPoint*>( feaprt );
                        assert( fixpt );

                        m_FixPointULocSlider.Update( fixpt->m_PosU.GetID() );
                        m_FixPointWLocSlider.Update( fixpt->m_PosW.GetID() );
                        m_FixPointMassToggle.Update( fixpt->m_FixPointMassFlag.GetID() );
                        m_FixPointMassSlider.Update( fixpt->m_FixPointMass.GetID() );

                        if ( fixpt->m_FixPointMassFlag() )
                        {
                            m_FixPointMassSlider.Activate();
                            m_FixPointMassUnit.Activate();
                        }
                        else
                        {
                            m_FixPointMassSlider.Deactivate();
                            m_FixPointMassUnit.Deactivate();
                        }

                        FeaPartDispGroup( &m_FixPointEditLayout );
                    }
                    else if ( feaprt->GetType() == vsp::FEA_DOME )
                    {
                        FeaDome* dome = dynamic_cast<FeaDome*>( feaprt );
                        assert( dome );

                        m_DomeARadSlider.Update( dome->m_Aradius.GetID() );
                        m_DomeBRadSlider.Update( dome->m_Bradius.GetID() );
                        m_DomeCRadSlider.Update( dome->m_Cradius.GetID() );

                        m_DomeXSlider.Update( dome->m_XLoc.GetID() );
                        m_DomeYSlider.Update( dome->m_YLoc.GetID() );
                        m_DomeZSlider.Update( dome->m_ZLoc.GetID() );

                        m_DomeXRotSlider.Update( dome->m_XRot.GetID() );
                        m_DomeYRotSlider.Update( dome->m_YRot.GetID() );
                        m_DomeZRotSlider.Update( dome->m_ZRot.GetID() );

                        m_DomeShellCapToggleGroup.Update( dome->m_IncludedElements.GetID() );

                        if ( dome->m_IncludedElements() == vsp::FEA_SHELL_AND_BEAM )
                        {
                            m_DomePropertyChoice.Activate();
                            m_DomeCapPropertyChoice.Activate();
                        }
                        else if ( dome->m_IncludedElements() == vsp::FEA_SHELL )
                        {
                            m_DomePropertyChoice.Activate();
                            m_DomeCapPropertyChoice.Deactivate();
                        }
                        else if ( dome->m_IncludedElements() == vsp::FEA_BEAM )
                        {
                            m_DomePropertyChoice.Deactivate();
                            m_DomeCapPropertyChoice.Activate();
                        }

                        FeaPartDispGroup( &m_DomeEditLayout );
                    }
                    else if ( feaprt->GetType() == vsp::FEA_RIB_ARRAY )
                    {
                        FeaRibArray* rib_array = dynamic_cast<FeaRibArray*>( feaprt );
                        assert( rib_array );

                        m_RibArrayPosTypeChoice.Update( rib_array->m_LocationParmType.GetID() );
                        m_RibArrayStartLocSlider.Update( rib_array->m_StartLocation.GetID() );
                        m_RibArraySpacingSlider.Update( rib_array->m_RibSpacing.GetID() );
                        m_RibArrayThetaSlider.Update( rib_array->m_Theta.GetID() );
                        m_RibArrayShellCapToggleGroup.Update( rib_array->m_IncludedElements.GetID() );
                        m_RibArrayPosNegDirToggleGroup.Update( rib_array->m_PositiveDirectionFlag.GetID() );

                        if ( rib_array->m_IncludedElements() == vsp::FEA_SHELL_AND_BEAM )
                        {
                            m_RibArrayPropertyChoice.Activate();
                            m_RibArrayCapPropertyChoice.Activate();
                        }
                        else if ( rib_array->m_IncludedElements() == vsp::FEA_SHELL )
                        {
                            m_RibArrayPropertyChoice.Activate();
                            m_RibArrayCapPropertyChoice.Deactivate();
                        }
                        else if ( rib_array->m_IncludedElements() == vsp::FEA_BEAM )
                        {
                            m_RibArrayPropertyChoice.Deactivate();
                            m_RibArrayCapPropertyChoice.Activate();
                        }

                        FeaPartDispGroup( &m_RibArrayEditLayout );
                    }
                    else if ( feaprt->GetType() == vsp::FEA_STIFFENER_ARRAY )
                    {
                        FeaStiffenerArray* stiffener_array = dynamic_cast<FeaStiffenerArray*>( feaprt );
                        assert( stiffener_array );

                        m_StiffenerArrayPosTypeChoice.Update( stiffener_array->m_LocationParmType.GetID() );
                        m_StiffenerArrayStartLocSlider.Update( stiffener_array->m_StartLocation.GetID() );
                        m_StiffenerArraySpacingSlider.Update( stiffener_array->m_StiffenerSpacing.GetID() );
                        m_StiffenerArrayPosNegDirToggleGroup.Update( stiffener_array->m_PositiveDirectionFlag.GetID() );

                        FeaPartDispGroup( &m_StiffenerArrayEditLayout );
                    }
                    else
                    {
                        Hide();
                        return false;
                    }
                }
                else
                {
                    Hide();
                    return false;
                }
            }
            else if ( StructureMgr.GetCurrPartIndex() >= structVec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() )
            {
                //===== SubSurface =====//
                SubSurface* subsurf = structVec[StructureMgr.GetCurrStructIndex()]->GetFeaSubSurf( StructureMgr.GetCurrPartIndex() - structVec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() );
                if ( subsurf )
                {
                    m_FeaPartNameInput.Update( subsurf->GetName() );
                    m_ShellCapToggleGroup.Update( subsurf->m_IncludedElements.GetID() );

                    if ( subsurf->GetType() == vsp::SS_LINE )
                    {
                        SSLine* ssline = dynamic_cast<SSLine*>( subsurf );
                        assert( ssline );

                        m_FeaSSLineConstToggleGroup.Update( ssline->m_ConstType.GetID() );
                        m_FeaSSLineTestToggleGroup.Update( ssline->m_TestType.GetID() );
                        m_FeaSSLineConstSlider.Update( ssline->m_ConstVal.GetID() );
                        m_FeaSSLineShellCapToggleGroup.Update( ssline->m_IncludedElements.GetID() );

                        if ( ssline->m_IncludedElements() == vsp::FEA_BEAM )
                        {
                            m_FeaSSLineCapPropertyChoice.Activate();
                            m_FeaSSLinePropertyChoice.Deactivate();
                        }
                        else if ( ssline->m_IncludedElements() == vsp::FEA_SHELL )
                        {
                            m_FeaSSLineCapPropertyChoice.Deactivate();
                            m_FeaSSLinePropertyChoice.Activate();
                        }
                        else if ( ssline->m_IncludedElements() == vsp::FEA_SHELL_AND_BEAM )
                        {
                            m_FeaSSLineCapPropertyChoice.Activate();
                            m_FeaSSLinePropertyChoice.Activate();
                        }

                        if ( ssline->m_TestType() == vsp::NONE )
                        {
                            m_FeaSSLinePropertyChoice.Deactivate();
                        }

                        FeaPartDispGroup( &m_FeaSSLineGroup );
                    }
                    else if ( subsurf->GetType() == vsp::SS_RECTANGLE )
                    {
                        SSRectangle* ssrec = dynamic_cast<SSRectangle*>( subsurf );
                        assert( ssrec );

                        m_FeaSSRecTestToggleGroup.Update( ssrec->m_TestType.GetID() );
                        m_FeaSSRecCentUSlider.Update( ssrec->m_CenterU.GetID() );
                        m_FeaSSRecCentWSlider.Update( ssrec->m_CenterW.GetID() );
                        m_FeaSSRecULenSlider.Update( ssrec->m_ULength.GetID() );
                        m_FeaSSRecWLenSlider.Update( ssrec->m_WLength.GetID() );
                        m_FeaSSRecThetaSlider.Update( ssrec->m_Theta.GetID() );
                        m_FeaSSRecShellCapToggleGroup.Update( ssrec->m_IncludedElements.GetID() );

                        if ( ssrec->m_IncludedElements() == vsp::FEA_BEAM )
                        {
                            m_FeaSSRecCapPropertyChoice.Activate();
                            m_FeaSSRecPropertyChoice.Deactivate();
                            m_FeaSSRecTestToggleGroup.Deactivate();
                            ssrec->m_TestType.Set( vsp::INSIDE ); // Inside tris must be set to be removed. Skin tris are saved/removed using FeaSkin parms
                        }
                        else if ( ssrec->m_IncludedElements() == vsp::FEA_SHELL )
                        {
                            m_FeaSSRecCapPropertyChoice.Deactivate();
                            m_FeaSSRecPropertyChoice.Activate();
                            m_FeaSSRecTestToggleGroup.Activate();
                        }
                        else if ( ssrec->m_IncludedElements() == vsp::FEA_SHELL_AND_BEAM )
                        {
                            m_FeaSSRecCapPropertyChoice.Activate();
                            m_FeaSSRecPropertyChoice.Activate();
                            m_FeaSSRecTestToggleGroup.Activate();
                        }

                        if ( ssrec->m_TestType() == vsp::NONE )
                        {
                            m_FeaSSRecPropertyChoice.Deactivate();
                        }

                        FeaPartDispGroup( &m_FeaSSRecGroup );
                    }
                    else if ( subsurf->GetType() == vsp::SS_ELLIPSE )
                    {
                        SSEllipse* ssell = dynamic_cast<SSEllipse*>( subsurf );
                        assert( ssell );

                        m_FeaSSEllTestToggleGroup.Update( ssell->m_TestType.GetID() );
                        m_FeaSSEllTessSlider.Update( ssell->m_Tess.GetID() );
                        m_FeaSSEllCentUSlider.Update( ssell->m_CenterU.GetID() );
                        m_FeaSSEllCentWSlider.Update( ssell->m_CenterW.GetID() );
                        m_FeaSSEllULenSlider.Update( ssell->m_ULength.GetID() );
                        m_FeaSSEllWLenSlider.Update( ssell->m_WLength.GetID() );
                        m_FeaSSEllThetaSlider.Update( ssell->m_Theta.GetID() );
                        m_FeaSSEllShellCapToggleGroup.Update( ssell->m_IncludedElements.GetID() );

                        if ( ssell->m_IncludedElements() == vsp::FEA_BEAM )
                        {
                            m_FeaSSEllCapPropertyChoice.Activate();
                            m_FeaSSEllPropertyChoice.Deactivate();
                            m_FeaSSEllTestToggleGroup.Deactivate();
                            ssell->m_TestType.Set( vsp::INSIDE ); // Inside tris must be set to be removed. Skin tris are saved/removed using FeaSkin parms
                        }
                        else if ( ssell->m_IncludedElements() == vsp::FEA_SHELL )
                        {
                            m_FeaSSEllCapPropertyChoice.Deactivate();
                            m_FeaSSEllPropertyChoice.Activate();
                            m_FeaSSEllTestToggleGroup.Activate();
                        }
                        else if ( ssell->m_IncludedElements() == vsp::FEA_SHELL_AND_BEAM )
                        {
                            m_FeaSSEllCapPropertyChoice.Activate();
                            m_FeaSSEllPropertyChoice.Activate();
                            m_FeaSSEllTestToggleGroup.Activate();
                        }

                        if ( ssell->m_TestType() == vsp::NONE )
                        {
                            m_FeaSSEllPropertyChoice.Deactivate();
                        }

                        FeaPartDispGroup( &m_FeaSSEllGroup );
                    }
                    else if ( subsurf->GetType() == vsp::SS_CONTROL )
                    {
                        SSControlSurf* sscon = dynamic_cast<SSControlSurf*>( subsurf );
                        assert( sscon );

                        m_FeaSSConTestToggleGroup.Update( sscon->m_TestType.GetID() );
                        m_FeaSSConUESlider.Update( sscon->m_UEnd.GetID() );
                        m_FeaSSConUSSlider.Update( sscon->m_UStart.GetID() );
                        m_FeaSSConSFracSlider.Update( sscon->m_StartLenFrac.GetID() );
                        m_FeaSSConSLenSlider.Update( sscon->m_StartLength.GetID() );
                        m_FeaSSConEFracSlider.Update( sscon->m_EndLenFrac.GetID() );
                        m_FeaSSConELenSlider.Update( sscon->m_EndLength.GetID() );
                        m_FeaSSConSAbsRelToggleGroup.Update( sscon->m_AbsRelFlag.GetID() );
                        m_FeaSSConSEConstButton.Update( sscon->m_ConstFlag.GetID() );
                        m_FeaSSConLEFlagButton.Update( sscon->m_LEFlag.GetID() );
                        m_FeaSSConShellCapToggleGroup.Update( sscon->m_IncludedElements.GetID() );
                        m_FeaSSConSAngleButton.Update( sscon->m_StartAngleFlag.GetID() );
                        m_FeaSSConEAngleButton.Update( sscon->m_EndAngleFlag.GetID() );
                        m_FeaSSConSAngleSlider.Update( sscon->m_StartAngle.GetID() );
                        m_FeaSSConEAngleSlider.Update( sscon->m_EndAngle.GetID() );

                        m_FeaSSConTessSlider.Update( sscon->m_Tess.GetID() );

                        if ( sscon->m_IncludedElements() == vsp::FEA_BEAM )
                        {
                            m_FeaSSConCapPropertyChoice.Activate();
                            m_FeaSSConPropertyChoice.Deactivate();
                            m_FeaSSConTestToggleGroup.Deactivate();
                            sscon->m_TestType.Set( vsp::INSIDE ); // Inside tris must be set to be removed. Skin tris are saved/removed using FeaSkin parms
                        }
                        else if ( sscon->m_IncludedElements() == vsp::FEA_SHELL )
                        {
                            m_FeaSSConCapPropertyChoice.Deactivate();
                            m_FeaSSConPropertyChoice.Activate();
                            m_FeaSSConTestToggleGroup.Activate();
                        }
                        else if ( sscon->m_IncludedElements() == vsp::FEA_SHELL_AND_BEAM )
                        {
                            m_FeaSSConCapPropertyChoice.Activate();
                            m_FeaSSConPropertyChoice.Activate();
                            m_FeaSSConTestToggleGroup.Activate();
                        }

                        if ( sscon->m_TestType() == vsp::NONE )
                        {
                            m_FeaSSConPropertyChoice.Deactivate();
                        }

                        if ( sscon->m_StartAngleFlag() )
                        {
                            m_FeaSSConSAngleSlider.Activate();
                        }
                        else
                        {
                            m_FeaSSConSAngleSlider.Deactivate();
                        }

                        m_FeaSSConSameAngleButton.Update( sscon->m_SameAngleFlag.GetID() );

                        if ( sscon->m_StartAngleFlag() && sscon->m_EndAngleFlag() )
                        {
                            m_FeaSSConSameAngleButton.Activate();
                        }
                        else
                        {
                            m_FeaSSConSameAngleButton.Deactivate();
                        }

                        if ( sscon->m_EndAngleFlag() && ( !sscon->m_SameAngleFlag() || ( !sscon->m_StartAngleFlag() && sscon->m_SameAngleFlag() ) ) )
                        {
                            m_FeaSSConEAngleSlider.Activate();
                        }
                        else
                        {
                            m_FeaSSConEAngleSlider.Deactivate();
                        }

                        m_FeaSSConSFracSlider.Deactivate();
                        m_FeaSSConSLenSlider.Deactivate();

                        m_FeaSSConEFracSlider.Deactivate();
                        m_FeaSSConELenSlider.Deactivate();

                        if ( sscon->m_AbsRelFlag() == vsp::ABS )
                        {
                            m_FeaSSConSLenSlider.Activate();

                            if ( !sscon->m_ConstFlag() )
                            {
                                m_FeaSSConELenSlider.Activate();
                            }
                        }
                        else
                        {
                            m_FeaSSConSFracSlider.Activate();

                            if ( !sscon->m_ConstFlag() )
                            {
                                m_FeaSSConEFracSlider.Activate();
                            }
                        }

                        m_FeaSSConSurfTypeChoice.Update( sscon->m_SurfType.GetID() );
                        FeaPartDispGroup( &m_FeaSSConGroup );
                    }
                    else if ( subsurf->GetType() == vsp::SS_LINE_ARRAY )
                    {
                        SSLineArray* sslinearray = dynamic_cast<SSLineArray*>( subsurf );
                        assert( sslinearray );

                        m_FeaSSLineArrayConstToggleGroup.Update( sslinearray->m_ConstType.GetID() );
                        m_FeaSSLineArrayPosNegDirToggleGroup.Update( sslinearray->m_PositiveDirectionFlag.GetID() );
                        m_FeaSSLineArraySpacingSlider.Update( sslinearray->m_Spacing.GetID() );
                        m_FeaSSLineArrayStartLocSlider.Update( sslinearray->m_StartLocation.GetID() );

                        FeaPartDispGroup( &m_FeaSSLineArrayGroup );
                    }
                    else
                    {
                        Hide();
                        return false;
                    }
                }
                else
                {
                    Hide();
                    return false;
                }
            }
            else
            {
                Hide();
                return false;
            }
        }
        else
        {
            Hide();
            return false;
        }
    }

    return true;
}

void FeaPartEditScreen::GuiDeviceCallBack( GuiDevice* device )
{
    assert( m_ScreenMgr );

    Vehicle*  veh = m_ScreenMgr->GetVehiclePtr();

    if ( !veh )
    {
        return;
    }

    if ( device == &m_FeaPartNameInput )
    {
        if ( StructureMgr.ValidTotalFeaStructInd( StructureMgr.GetCurrStructIndex() ) )
        {
            vector < FeaStructure* > structvec = StructureMgr.GetAllFeaStructs();

            if ( StructureMgr.GetCurrPartIndex() < structvec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() )
            {
                FeaPart* feaprt = structvec[StructureMgr.GetCurrStructIndex()]->GetFeaPart( StructureMgr.GetCurrPartIndex() );

                if ( feaprt )
                {
                    feaprt->SetName( m_FeaPartNameInput.GetString() );
                }
            }
            else if ( StructureMgr.GetCurrPartIndex() >= structvec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() )
            {
                SubSurface* subsurf = structvec[StructureMgr.GetCurrStructIndex()]->GetFeaSubSurf( StructureMgr.GetCurrPartIndex() - structvec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() );

                if ( subsurf )
                {
                    subsurf->SetName( m_FeaPartNameInput.GetString() );
                }
            }
        }
    }
    else if ( device == &m_SliceOrientationChoice )
    {
        StructureMgr.SetFeaSliceOrientIndex( m_SliceOrientationChoice.GetVal() );
    }
    else if ( device == &m_SkinPropertyChoice || device == &m_SlicePropertyChoice || device == &m_RibPropertyChoice || device == &m_SparPropertyChoice
              || device == &m_DomePropertyChoice || device == &m_RibArrayPropertyChoice || device == &m_FeaSSLinePropertyChoice || device == &m_FeaSSRecPropertyChoice
              || device == &m_FeaSSEllPropertyChoice || device == &m_FeaSSConPropertyChoice )
    {
        Choice* selected_choice = dynamic_cast<Choice*>( device );
        assert( selected_choice );

        SetFeaPartPropertyIndex( selected_choice );
    }
    else if ( device == &m_SliceCapPropertyChoice || device == &m_RibCapPropertyChoice || device == &m_SparCapPropertyChoice || device == &m_DomeCapPropertyChoice
              || device == &m_RibArrayCapPropertyChoice || device == &m_StiffenerArrayCapPropertyChoice || device == &m_FeaSSLineCapPropertyChoice 
              || device == &m_FeaSSRecCapPropertyChoice || device == &m_FeaSSEllCapPropertyChoice || device == &m_FeaSSConCapPropertyChoice || device == &m_FeaSSLineArrayCapPropertyChoice )
    {
        Choice* selected_choice = dynamic_cast<Choice*>( device );
        assert( selected_choice );

        SetCapPropertyIndex( selected_choice );
    }
    else if ( device == &m_RibPerpendicularEdgeChoice )
    {
        if ( StructureMgr.ValidTotalFeaStructInd( StructureMgr.GetCurrStructIndex() ) )
        {
            vector < FeaStructure* > structVec = StructureMgr.GetAllFeaStructs();

            if ( StructureMgr.GetCurrPartIndex() < structVec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() )
            {
                FeaPart* feaprt = structVec[StructureMgr.GetCurrStructIndex()]->GetFeaPart( StructureMgr.GetCurrPartIndex() );

                if ( feaprt )
                {
                    if ( feaprt->GetType() == vsp::FEA_RIB )
                    {
                        FeaRib* rib = dynamic_cast<FeaRib*>( feaprt );
                        assert( rib );

                        rib->SetPerpendicularEdgeID( m_PerpendicularEdgeVec[m_RibPerpendicularEdgeChoice.GetVal()] );
                    }
                }
            }
        }
    }
    else if ( device == &m_FixPointParentSurfChoice )
    {
        if ( StructureMgr.ValidTotalFeaStructInd( StructureMgr.GetCurrStructIndex() ) )
        {
            vector < FeaStructure* > structVec = StructureMgr.GetAllFeaStructs();

            FeaPart* parent_feaprt = StructureMgr.GetFeaPart( m_FixPointParentIDVec[m_FixPointParentSurfChoice.GetVal()] );

            if ( StructureMgr.GetCurrPartIndex() < structVec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() )
            {
                FeaPart* feaprt = structVec[StructureMgr.GetCurrStructIndex()]->GetFeaPart( StructureMgr.GetCurrPartIndex() );

                if ( feaprt && parent_feaprt )
                {
                    if ( feaprt->GetType() == vsp::FEA_FIX_POINT )
                    {
                        FeaFixPoint* fixpt = dynamic_cast<FeaFixPoint*>( feaprt );
                        assert( fixpt );

                        fixpt->m_ParentFeaPartID = parent_feaprt->GetID();
                    }
                }
            }
        }
    }
    else if ( device == &m_RibArrayPerpEdgeChoice )
    {
        if ( StructureMgr.ValidTotalFeaStructInd( StructureMgr.GetCurrStructIndex() ) )
        {
            vector < FeaStructure* > structVec = StructureMgr.GetAllFeaStructs();

            if ( StructureMgr.GetCurrPartIndex() < structVec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() )
            {
                FeaPart* feaprt = structVec[StructureMgr.GetCurrStructIndex()]->GetFeaPart( StructureMgr.GetCurrPartIndex() );

                if ( feaprt )
                {
                    if ( feaprt->GetType() == vsp::FEA_RIB_ARRAY )
                    {
                        FeaRibArray* rib_array = dynamic_cast<FeaRibArray*>( feaprt );
                        assert( rib_array );

                        rib_array->SetPerpendicularEdgeID( m_PerpendicularEdgeVec[m_RibArrayPerpEdgeChoice.GetVal()] );
                    }
                }
            }
        }
    }
    else if ( device == &m_IndividualizeRibArrayButton )
    {
        if ( StructureMgr.ValidTotalFeaStructInd( StructureMgr.GetCurrStructIndex() ) )
        {
            vector < FeaStructure* > structVec = StructureMgr.GetAllFeaStructs();

            if ( StructureMgr.GetCurrPartIndex() < structVec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() )
            {
                FeaPart* feaprt = structVec[StructureMgr.GetCurrStructIndex()]->GetFeaPart( StructureMgr.GetCurrPartIndex() );

                if ( feaprt )
                {
                    if ( feaprt->GetType() == vsp::FEA_RIB_ARRAY )
                    {
                        structVec[StructureMgr.GetCurrStructIndex()]->IndividualizeRibArray( StructureMgr.GetCurrPartIndex() );
                    }
                }
            }
        }
    }
    else if ( device == &m_IndividualizeStiffenerArrayButton )
    {
        if ( StructureMgr.ValidTotalFeaStructInd( StructureMgr.GetCurrStructIndex() ) )
        {
            vector < FeaStructure* > structVec = StructureMgr.GetAllFeaStructs();

            if ( StructureMgr.GetCurrPartIndex() < structVec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() )
            {
                FeaPart* feaprt = structVec[StructureMgr.GetCurrStructIndex()]->GetFeaPart( StructureMgr.GetCurrPartIndex() );

                if ( feaprt )
                {
                    if ( feaprt->GetType() == vsp::FEA_STIFFENER_ARRAY )
                    {
                        structVec[StructureMgr.GetCurrStructIndex()]->IndividualizeStiffenerArray( StructureMgr.GetCurrPartIndex() );
                    }
                }
            }
        }
    }

    m_ScreenMgr->SetUpdateFlag( true );
}

void FeaPartEditScreen::CloseCallBack( Fl_Widget *w )
{
    assert( m_ScreenMgr );

    Hide();
}

void FeaPartEditScreen::UpdateFeaPropertyChoice()
{
    //==== Property Choice ====//
    m_SkinPropertyChoice.ClearItems();
    m_SlicePropertyChoice.ClearItems();
    m_RibPropertyChoice.ClearItems();
    m_SparPropertyChoice.ClearItems();
    m_DomePropertyChoice.ClearItems();
    m_RibArrayPropertyChoice.ClearItems();
    m_FeaSSLinePropertyChoice.ClearItems();
    m_FeaSSRecPropertyChoice.ClearItems();
    m_FeaSSEllPropertyChoice.ClearItems();
    m_FeaSSConPropertyChoice.ClearItems();

    m_SparCapPropertyChoice.ClearItems();
    m_SliceCapPropertyChoice.ClearItems();
    m_RibCapPropertyChoice.ClearItems();
    m_DomeCapPropertyChoice.ClearItems();
    m_RibArrayCapPropertyChoice.ClearItems();
    m_StiffenerArrayCapPropertyChoice.ClearItems();

    m_FeaSSLineCapPropertyChoice.ClearItems();
    m_FeaSSRecCapPropertyChoice.ClearItems();
    m_FeaSSEllCapPropertyChoice.ClearItems();
    m_FeaSSConCapPropertyChoice.ClearItems();
    m_FeaSSLineArrayCapPropertyChoice.ClearItems();

    Vehicle*  veh = m_ScreenMgr->GetVehiclePtr();

    if ( veh )
    {
        vector< FeaProperty* > property_vec = StructureMgr.GetFeaPropertyVec();

        for ( int i = 0; i < property_vec.size(); ++i )
        {
            m_SkinPropertyChoice.AddItem( string( property_vec[i]->GetName() ) );
            m_SlicePropertyChoice.AddItem( string( property_vec[i]->GetName() ) );
            m_RibPropertyChoice.AddItem( string( property_vec[i]->GetName() ) );
            m_SparPropertyChoice.AddItem( string( property_vec[i]->GetName() ) );
            m_DomePropertyChoice.AddItem( string( property_vec[i]->GetName() ) );
            m_RibArrayPropertyChoice.AddItem( string( property_vec[i]->GetName() ) );
            m_FeaSSLinePropertyChoice.AddItem( string( property_vec[i]->GetName() ) );
            m_FeaSSRecPropertyChoice.AddItem( string( property_vec[i]->GetName() ) );
            m_FeaSSEllPropertyChoice.AddItem( string( property_vec[i]->GetName() ) );
            m_FeaSSConPropertyChoice.AddItem( string( property_vec[i]->GetName() ) );

            m_SparCapPropertyChoice.AddItem( string( property_vec[i]->GetName() ) );
            m_SliceCapPropertyChoice.AddItem( string( property_vec[i]->GetName() ) );
            m_RibCapPropertyChoice.AddItem( string( property_vec[i]->GetName() ) );
            m_DomeCapPropertyChoice.AddItem( string( property_vec[i]->GetName() ) );
            m_RibArrayCapPropertyChoice.AddItem( string( property_vec[i]->GetName() ) );
            m_StiffenerArrayCapPropertyChoice.AddItem( string( property_vec[i]->GetName() ) );

            m_FeaSSLineCapPropertyChoice.AddItem( string( property_vec[i]->GetName() ) );
            m_FeaSSRecCapPropertyChoice.AddItem( string( property_vec[i]->GetName() ) );
            m_FeaSSEllCapPropertyChoice.AddItem( string( property_vec[i]->GetName() ) );
            m_FeaSSConCapPropertyChoice.AddItem( string( property_vec[i]->GetName() ) );
            m_FeaSSLineArrayCapPropertyChoice.AddItem( string( property_vec[i]->GetName() ) );

            if ( property_vec[i]->m_FeaPropertyType() == vsp::FEA_SHELL )
            {
                m_SkinPropertyChoice.SetFlag( i, 0 );
                m_SlicePropertyChoice.SetFlag( i, 0 );
                m_RibPropertyChoice.SetFlag( i, 0 );
                m_SparPropertyChoice.SetFlag( i, 0 );
                m_DomePropertyChoice.SetFlag( i, 0 );
                m_RibArrayPropertyChoice.SetFlag( i, 0 );
                m_FeaSSLinePropertyChoice.SetFlag( i, 0 );
                m_FeaSSRecPropertyChoice.SetFlag( i, 0 );
                m_FeaSSEllPropertyChoice.SetFlag( i, 0 );
                m_FeaSSConPropertyChoice.SetFlag( i, 0 );

                m_SparCapPropertyChoice.SetFlag( i, FL_MENU_INACTIVE );
                m_SliceCapPropertyChoice.SetFlag( i, FL_MENU_INACTIVE );
                m_RibCapPropertyChoice.SetFlag( i, FL_MENU_INACTIVE );
                m_DomeCapPropertyChoice.SetFlag( i, FL_MENU_INACTIVE );
                m_RibArrayCapPropertyChoice.SetFlag( i, FL_MENU_INACTIVE );
                m_StiffenerArrayCapPropertyChoice.SetFlag( i, FL_MENU_INACTIVE );

                m_FeaSSLineCapPropertyChoice.SetFlag( i, FL_MENU_INACTIVE );
                m_FeaSSRecCapPropertyChoice.SetFlag( i, FL_MENU_INACTIVE );
                m_FeaSSEllCapPropertyChoice.SetFlag( i, FL_MENU_INACTIVE );
                m_FeaSSConCapPropertyChoice.SetFlag( i, FL_MENU_INACTIVE );
                m_FeaSSLineArrayCapPropertyChoice.SetFlag( i, FL_MENU_INACTIVE );
            }
            else if ( property_vec[i]->m_FeaPropertyType() == vsp::FEA_BEAM )
            {
                m_SkinPropertyChoice.SetFlag( i, FL_MENU_INACTIVE );
                m_SlicePropertyChoice.SetFlag( i, FL_MENU_INACTIVE );
                m_RibPropertyChoice.SetFlag( i, FL_MENU_INACTIVE );
                m_SparPropertyChoice.SetFlag( i, FL_MENU_INACTIVE );
                m_DomePropertyChoice.SetFlag( i, FL_MENU_INACTIVE );
                m_RibArrayPropertyChoice.SetFlag( i, FL_MENU_INACTIVE );
                m_FeaSSLinePropertyChoice.SetFlag( i, FL_MENU_INACTIVE );
                m_FeaSSRecPropertyChoice.SetFlag( i, FL_MENU_INACTIVE );
                m_FeaSSEllPropertyChoice.SetFlag( i, FL_MENU_INACTIVE );
                m_FeaSSConPropertyChoice.SetFlag( i, FL_MENU_INACTIVE );

                m_SparCapPropertyChoice.SetFlag( i, 0 );
                m_SliceCapPropertyChoice.SetFlag( i, 0 );
                m_RibCapPropertyChoice.SetFlag( i, 0 );
                m_DomeCapPropertyChoice.SetFlag( i, 0 );
                m_RibArrayCapPropertyChoice.SetFlag( i, 0 );
                m_StiffenerArrayCapPropertyChoice.SetFlag( i, 0 );

                m_FeaSSLineCapPropertyChoice.SetFlag( i, 0 );
                m_FeaSSRecCapPropertyChoice.SetFlag( i, 0 );
                m_FeaSSEllCapPropertyChoice.SetFlag( i, 0 );
                m_FeaSSConCapPropertyChoice.SetFlag( i, 0 );
                m_FeaSSLineArrayCapPropertyChoice.SetFlag( i, 0 );
            }
        }

        m_SkinPropertyChoice.UpdateItems();
        m_SlicePropertyChoice.UpdateItems();
        m_RibPropertyChoice.UpdateItems();
        m_SparPropertyChoice.UpdateItems();
        m_DomePropertyChoice.UpdateItems();
        m_RibArrayPropertyChoice.UpdateItems();
        m_FeaSSLinePropertyChoice.UpdateItems();
        m_FeaSSRecPropertyChoice.UpdateItems();
        m_FeaSSEllPropertyChoice.UpdateItems();
        m_FeaSSConPropertyChoice.UpdateItems();

        m_SparCapPropertyChoice.UpdateItems();
        m_SliceCapPropertyChoice.UpdateItems();
        m_RibCapPropertyChoice.UpdateItems();
        m_DomeCapPropertyChoice.UpdateItems();
        m_RibArrayCapPropertyChoice.UpdateItems();
        m_StiffenerArrayCapPropertyChoice.UpdateItems();

        m_FeaSSLineCapPropertyChoice.UpdateItems();
        m_FeaSSRecCapPropertyChoice.UpdateItems();
        m_FeaSSEllCapPropertyChoice.UpdateItems();
        m_FeaSSConCapPropertyChoice.UpdateItems();
        m_FeaSSLineArrayCapPropertyChoice.UpdateItems();

        if ( StructureMgr.ValidTotalFeaStructInd( StructureMgr.GetCurrStructIndex() ) )
        {
            vector < FeaStructure* > structvec = StructureMgr.GetAllFeaStructs();

            if ( StructureMgr.GetCurrPartIndex() < structvec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() )
            {
                FeaPart* feaprt = structvec[StructureMgr.GetCurrStructIndex()]->GetFeaPart( StructureMgr.GetCurrPartIndex() );

                if ( feaprt )
                {
                    // Update all FeaPart Property Choices ( Only Selected Part Visible )
                    m_SkinPropertyChoice.SetVal( feaprt->m_FeaPropertyIndex() );
                    m_SlicePropertyChoice.SetVal( feaprt->m_FeaPropertyIndex() );
                    m_RibPropertyChoice.SetVal( feaprt->m_FeaPropertyIndex() );
                    m_SparPropertyChoice.SetVal( feaprt->m_FeaPropertyIndex() );
                    m_DomePropertyChoice.SetVal( feaprt->m_FeaPropertyIndex() );
                    m_RibArrayPropertyChoice.SetVal( feaprt->m_FeaPropertyIndex() );

                    m_SparCapPropertyChoice.SetVal( feaprt->m_CapFeaPropertyIndex() );
                    m_SliceCapPropertyChoice.SetVal( feaprt->m_CapFeaPropertyIndex() );
                    m_RibCapPropertyChoice.SetVal( feaprt->m_CapFeaPropertyIndex() );
                    m_DomeCapPropertyChoice.SetVal( feaprt->m_CapFeaPropertyIndex() );
                    m_RibArrayCapPropertyChoice.SetVal( feaprt->m_CapFeaPropertyIndex() );
                    m_StiffenerArrayCapPropertyChoice.SetVal( feaprt->m_CapFeaPropertyIndex() );
                }
            }
            else if ( StructureMgr.GetCurrPartIndex() >= structvec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() )
            {
                SubSurface* subsurf = structvec[StructureMgr.GetCurrStructIndex()]->GetFeaSubSurf( StructureMgr.GetCurrPartIndex() - structvec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() );

                if ( subsurf )
                {
                    // Update all FeaSubSurface Property Choices ( Only Selected Part Visible )
                    m_FeaSSLinePropertyChoice.SetVal( subsurf->m_FeaPropertyIndex() );
                    m_FeaSSRecPropertyChoice.SetVal( subsurf->m_FeaPropertyIndex() );
                    m_FeaSSEllPropertyChoice.SetVal( subsurf->m_FeaPropertyIndex() );
                    m_FeaSSConPropertyChoice.SetVal( subsurf->m_FeaPropertyIndex() );

                    m_FeaSSLineCapPropertyChoice.SetVal( subsurf->m_CapFeaPropertyIndex() );
                    m_FeaSSRecCapPropertyChoice.SetVal( subsurf->m_CapFeaPropertyIndex() );
                    m_FeaSSEllCapPropertyChoice.SetVal( subsurf->m_CapFeaPropertyIndex() );
                    m_FeaSSConCapPropertyChoice.SetVal( subsurf->m_CapFeaPropertyIndex() );
                    m_FeaSSLineArrayCapPropertyChoice.SetVal( subsurf->m_CapFeaPropertyIndex() );
                }
            }
        }
    }
}

void FeaPartEditScreen::UpdateFixPointParentChoice()
{
    //==== FixPoint Parent Surf Choice ====//
    m_FixPointParentSurfChoice.ClearItems();
    m_FixPointParentIDVec.clear();

    if ( StructureMgr.ValidTotalFeaStructInd( StructureMgr.GetCurrStructIndex() ) )
    {
        vector< FeaStructure* > structVec = StructureMgr.GetAllFeaStructs();
        vector<FeaPart*> feaprt_vec = structVec[StructureMgr.GetCurrStructIndex()]->GetFeaPartVec();

        for ( size_t i = 0; i < feaprt_vec.size(); i++ )
        {

            if ( !structVec[StructureMgr.GetCurrStructIndex()]->FeaPartIsFixPoint( i ) )
            {
                m_FixPointParentSurfChoice.AddItem( string( feaprt_vec[i]->GetName() + "_Surf" ) );
                m_FixPointParentIDVec.push_back( feaprt_vec[i]->GetID() );
            }
        }

        m_FixPointParentSurfChoice.UpdateItems();

        if ( StructureMgr.GetCurrPartIndex() < structVec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() )
        {
            FeaPart* feaprt = structVec[StructureMgr.GetCurrStructIndex()]->GetFeaPart( StructureMgr.GetCurrPartIndex() );

            if ( feaprt )
            {
                if ( feaprt->GetType() == vsp::FEA_FIX_POINT )
                {
                    FeaFixPoint* fixpt = dynamic_cast<FeaFixPoint*>( feaprt );
                    assert( fixpt );

                    FeaPart* parent_feaprt = StructureMgr.GetFeaPart( fixpt->m_ParentFeaPartID );

                    if ( parent_feaprt )
                    {
                        int parent_index = structVec[StructureMgr.GetCurrStructIndex()]->GetFeaPartIndex( parent_feaprt );

                        if ( structVec[StructureMgr.GetCurrStructIndex()]->ValidFeaPartInd( parent_index ) )
                        {
                            m_FixPointParentSurfChoice.SetVal( parent_index );
                        }
                    }
                    else
                    {
                        // Set skin as parent surface if undefined
                        if ( structVec[StructureMgr.GetCurrStructIndex()]->GetFeaSkin() )
                        {
                            fixpt->m_ParentFeaPartID = structVec[StructureMgr.GetCurrStructIndex()]->GetFeaSkin()->GetID();
                        }
                    }
                }
            }
        }
    }
}

void FeaPartEditScreen::UpdatePerpendicularRibChoice()
{
    //==== Perpendicular Rib Choice ====//
    m_RibPerpendicularEdgeChoice.ClearItems();
    m_RibArrayPerpEdgeChoice.ClearItems();
    m_PerpendicularEdgeVec.clear();

    m_RibPerpendicularEdgeChoice.AddItem( "None" );
    m_RibArrayPerpEdgeChoice.AddItem( "None" );
    m_PerpendicularEdgeVec.push_back( "None" );
    m_RibPerpendicularEdgeChoice.AddItem( "Leading Edge" );
    m_RibArrayPerpEdgeChoice.AddItem( "Leading Edge" );
    m_PerpendicularEdgeVec.push_back( "Leading Edge" );
    m_RibPerpendicularEdgeChoice.AddItem( "Trailing Edge" );
    m_RibArrayPerpEdgeChoice.AddItem( "Trailing Edge" );
    m_PerpendicularEdgeVec.push_back( "Trailing Edge" );

    Vehicle*  veh = m_ScreenMgr->GetVehiclePtr();

    if ( veh )
    {
        if ( StructureMgr.ValidTotalFeaStructInd( StructureMgr.GetCurrStructIndex() ) )
        {
            FeaStructure* curr_struct = StructureMgr.GetAllFeaStructs()[StructureMgr.GetCurrStructIndex()];

            if ( curr_struct )
            {
                vector < FeaPart* > part_vec = curr_struct->GetFeaPartVec();

                for ( size_t i = 1; i < part_vec.size(); i++ )
                {
                    if ( part_vec[i]->GetType() == vsp::FEA_SPAR )
                    {
                        m_RibPerpendicularEdgeChoice.AddItem( part_vec[i]->GetName() );
                        m_RibArrayPerpEdgeChoice.AddItem( part_vec[i]->GetName() );
                        m_PerpendicularEdgeVec.push_back( part_vec[i]->GetID() );
                    }
                }

                m_RibPerpendicularEdgeChoice.UpdateItems();
                m_RibArrayPerpEdgeChoice.UpdateItems();

                if ( StructureMgr.GetCurrPartIndex() < curr_struct->NumFeaParts() )
                {
                    FeaPart* feaprt = curr_struct->GetFeaPart( StructureMgr.GetCurrPartIndex() );

                    if ( feaprt )
                    {
                        if ( feaprt->GetType() == vsp::FEA_RIB )
                        {
                            FeaRib* rib = dynamic_cast<FeaRib*>( feaprt );
                            assert( rib );

                            for ( size_t k = 0; k < m_PerpendicularEdgeVec.size(); k++ )
                            {
                                if ( rib->GetPerpendicularEdgeID() == m_PerpendicularEdgeVec[k] )
                                {
                                    m_RibPerpendicularEdgeChoice.SetVal( k );
                                    break;
                                }
                            }
                        }
                        else if ( feaprt->GetType() == vsp::FEA_RIB_ARRAY )
                        {
                            FeaRibArray* rib_array = dynamic_cast<FeaRibArray*>( feaprt );
                            assert( rib_array );

                            for ( size_t k = 0; k < m_PerpendicularEdgeVec.size(); k++ )
                            {
                                if ( rib_array->GetPerpendicularEdgeID() == m_PerpendicularEdgeVec[k] )
                                {
                                    m_RibArrayPerpEdgeChoice.SetVal( k );
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void FeaPartEditScreen::FeaPartDispGroup( GroupLayout* group )
{
    if ( m_CurFeaPartDispGroup == group && group )
    {
        return;
    }

    m_SkinEditLayout.Hide();
    m_SliceEditLayout.Hide();
    m_SliceEditLayout.Hide();
    m_RibEditLayout.Hide();
    m_SparEditLayout.Hide();
    m_FixPointEditLayout.Hide();
    m_DomeEditLayout.Hide();
    m_RibArrayEditLayout.Hide();
    m_StiffenerArrayEditLayout.Hide();

    m_FeaSSLineGroup.Hide();
    m_FeaSSRecGroup.Hide();
    m_FeaSSEllGroup.Hide();
    m_FeaSSConGroup.Hide();
    m_FeaSSLineArrayGroup.Hide();

    m_CurFeaPartDispGroup = group;

    if ( group )
    {
        group->Show();
    }
}

void FeaPartEditScreen::SetFeaPartPropertyIndex( Choice* property_choice )
{
    if ( StructureMgr.ValidTotalFeaStructInd( StructureMgr.GetCurrStructIndex() ) )
    {
        vector < FeaStructure* > structvec = StructureMgr.GetAllFeaStructs();

        if ( StructureMgr.GetCurrPartIndex() < structvec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() )
        {
            FeaPart* feaprt = structvec[StructureMgr.GetCurrStructIndex()]->GetFeaPart( StructureMgr.GetCurrPartIndex() );

            if ( feaprt )
            {
                feaprt->m_FeaPropertyIndex.Set( property_choice->GetVal() );
            }
        }
        else if ( StructureMgr.GetCurrPartIndex() >= structvec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() )
        {
            SubSurface* subsurf = structvec[StructureMgr.GetCurrStructIndex()]->GetFeaSubSurf( StructureMgr.GetCurrPartIndex() - structvec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() );

            if ( subsurf )
            {
                subsurf->m_FeaPropertyIndex.Set( property_choice->GetVal() );
            }
        }
    }
}

void FeaPartEditScreen::SetCapPropertyIndex( Choice* property_choice )
{
    if ( StructureMgr.ValidTotalFeaStructInd( StructureMgr.GetCurrStructIndex() ) )
    {
        vector < FeaStructure* > structvec = StructureMgr.GetAllFeaStructs();

        if ( StructureMgr.GetCurrPartIndex() < structvec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() )
        {
            FeaPart* feaprt = structvec[StructureMgr.GetCurrStructIndex()]->GetFeaPart( StructureMgr.GetCurrPartIndex() );

            if ( feaprt )
            {
                feaprt->m_CapFeaPropertyIndex.Set( property_choice->GetVal() );
            }
        }
        else if ( StructureMgr.GetCurrPartIndex() >= structvec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() )
        {
            SubSurface* subsurf = structvec[StructureMgr.GetCurrStructIndex()]->GetFeaSubSurf( StructureMgr.GetCurrPartIndex() - structvec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() );

            if ( subsurf )
            {
                subsurf->m_CapFeaPropertyIndex.Set( property_choice->GetVal() );
            }
        }
    }
}

void FeaPartEditScreen::UpdateUnitLabels()
{
    Vehicle* veh = m_ScreenMgr->GetVehiclePtr();

    if ( veh )
    {
        string mass_unit, dist_unit;
        string squared( 1, 178 );

        switch ( veh->m_StructUnit() )
        {
        case vsp::SI_UNIT:
        mass_unit = "kg";
        dist_unit = "m";
        break;

        case vsp::CGS_UNIT:
        mass_unit = "g";
        dist_unit = "cm";
        break;

        case vsp::MPA_UNIT:
        mass_unit = "tonne"; // or Mg/
        dist_unit = "mm";
        break;

        case vsp::BFT_UNIT:
        mass_unit = "slug";
        dist_unit = "ft";
        break;

        case vsp::BIN_UNIT:
        mass_unit = "lbf*sec" + squared + "/in";
        dist_unit = "in";
        break;
        }

        m_FixPointMassUnit.GetFlButton()->copy_label( mass_unit.c_str() );

        m_DomeARadUnit.GetFlButton()->copy_label( dist_unit.c_str() );
        m_DomeBRadUnit.GetFlButton()->copy_label( dist_unit.c_str() );
        m_DomeCRadUnit.GetFlButton()->copy_label( dist_unit.c_str() );
        m_DomeXUnit.GetFlButton()->copy_label( dist_unit.c_str() );
        m_DomeYUnit.GetFlButton()->copy_label( dist_unit.c_str() );
        m_DomeZUnit.GetFlButton()->copy_label( dist_unit.c_str() );

        if ( StructureMgr.ValidTotalFeaStructInd( StructureMgr.GetCurrStructIndex() ) )
        {
            vector< FeaStructure* > structVec = StructureMgr.GetAllFeaStructs();

            if ( StructureMgr.GetCurrPartIndex() < structVec[StructureMgr.GetCurrStructIndex()]->NumFeaParts() )
            {
                FeaPart* feaprt = structVec[StructureMgr.GetCurrStructIndex()]->GetFeaPart( StructureMgr.GetCurrPartIndex() );
                if ( feaprt )
                {
                    if ( feaprt->GetType() == vsp::FEA_SLICE || feaprt->GetType() == vsp::FEA_RIB || feaprt->GetType() == vsp::FEA_SPAR || feaprt->GetType() == vsp::FEA_RIB_ARRAY || feaprt->GetType() == vsp::FEA_STIFFENER_ARRAY ) // TODO: Switch to different check
                    {
                        if ( feaprt->m_LocationParmType() == LENGTH )
                        {
                            m_SlicePosUnit.GetFlButton()->copy_label( dist_unit.c_str() );
                            m_SparPosUnit.GetFlButton()->copy_label( dist_unit.c_str() );
                            m_RibPosUnit.GetFlButton()->copy_label( dist_unit.c_str() );
                            m_RibArrayStartLocUnit.GetFlButton()->copy_label( dist_unit.c_str() );
                            m_RibArrayPosUnit.GetFlButton()->copy_label( dist_unit.c_str() );
                            m_StiffenerArrayStartLocUnit.GetFlButton()->copy_label( dist_unit.c_str() );
                            m_StiffenerArrayPosUnit.GetFlButton()->copy_label( dist_unit.c_str() );
                        }
                        else if( feaprt->m_LocationParmType() == PERCENT )
                        {
                            m_SlicePosUnit.GetFlButton()->copy_label( "%" );
                            m_SparPosUnit.GetFlButton()->copy_label( "%" );
                            m_RibPosUnit.GetFlButton()->copy_label( "%" );
                            m_RibArrayStartLocUnit.GetFlButton()->copy_label( "%" );
                            m_RibArrayPosUnit.GetFlButton()->copy_label( "%" );
                            m_StiffenerArrayStartLocUnit.GetFlButton()->copy_label( "%" );
                            m_StiffenerArrayPosUnit.GetFlButton()->copy_label( "%" );
                        }
                    }
                }
            }
        }
    }
}