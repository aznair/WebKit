/*
 * Copyright (C) 2007 Alexey Proskuryakov <ap@nypop.com>.
 * Copyright (C) 2008-2023 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2009 Jeff Schiller <codedread@gmail.com>
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "ApplePayButtonSystemImage.h"
#include "CSSCalcValue.h"
#include "CSSPrimitiveValue.h"
#include "CSSToLengthConversionData.h"
#include "CSSValueKeywords.h"
#include "GraphicsTypes.h"
#include "Length.h"
#include "LineClampValue.h"
#include "RenderStyleConstants.h"
#include "SVGRenderStyleDefs.h"
#include "ScrollTypes.h"
#include "TextFlags.h"
#include "ThemeTypes.h"
#include "TouchAction.h"
#include "UnicodeBidi.h"
#include "WritingMode.h"
#include <wtf/MathExtras.h>
#include <wtf/OptionSet.h>

namespace WebCore {

#define EMIT_TO_CSS_SWITCH_CASE(VALUE) case TYPE::VALUE: return CSSValue##VALUE;
#define EMIT_FROM_CSS_SWITCH_CASE(VALUE) case CSSValue##VALUE: return TYPE::VALUE;

#define DEFINE_TO_FROM_CSS_VALUE_ID_FUNCTIONS \
constexpr CSSValueID toCSSValueID(TYPE value) { switch (value) { FOR_EACH(EMIT_TO_CSS_SWITCH_CASE) } } \
template<> constexpr TYPE fromCSSValueID(CSSValueID value) { switch (value) { FOR_EACH(EMIT_FROM_CSS_SWITCH_CASE) \
    default: break; } ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT(); return { }; }

template<> inline Ref<CSSPrimitiveValue> CSSPrimitiveValue::create(const LineClampValue& value)
{
    return create(value.value(), value.isPercentage() ? CSSUnitType::CSS_PERCENTAGE : CSSUnitType::CSS_INTEGER);
}

template<> inline CSSPrimitiveValue::operator LineClampValue() const
{
    if (primitiveType() == CSSUnitType::CSS_INTEGER)
        return LineClampValue(value<int>(), LineClamp::LineCount);

    if (primitiveType() == CSSUnitType::CSS_PERCENTAGE)
        return LineClampValue(value<int>(), LineClamp::Percentage);

    ASSERT_NOT_REACHED();
    return LineClampValue();
}

#define TYPE ReflectionDirection
#define FOR_EACH(CASE) CASE(Above) CASE(Below) CASE(Left) CASE(Right)
DEFINE_TO_FROM_CSS_VALUE_ID_FUNCTIONS
#undef TYPE
#undef FOR_EACH

#define TYPE ColumnFill
#define FOR_EACH(CASE) CASE(Auto) CASE(Balance)
DEFINE_TO_FROM_CSS_VALUE_ID_FUNCTIONS
#undef TYPE
#undef FOR_EACH

#define TYPE ColumnSpan
#define FOR_EACH(CASE) CASE(All) CASE(None)
DEFINE_TO_FROM_CSS_VALUE_ID_FUNCTIONS
#undef TYPE
#undef FOR_EACH

template<> inline CSSPrimitiveValue::operator ColumnSpan() const
{
    // Map 1 to none for compatibility reasons.
    if ((primitiveUnitType() == CSSUnitType::CSS_NUMBER || primitiveUnitType() == CSSUnitType::CSS_INTEGER) && m_value.num == 1)
        return ColumnSpan::None;
    return fromCSSValueID<ColumnSpan>(valueID());
}

#define TYPE PrintColorAdjust
#define FOR_EACH(CASE) CASE(Exact) CASE(Economy)
DEFINE_TO_FROM_CSS_VALUE_ID_FUNCTIONS
#undef TYPE
#undef FOR_EACH

constexpr CSSValueID toCSSValueID(BorderStyle e)
{
    switch (e) {
    case BorderStyle::None:
        return CSSValueNone;
    case BorderStyle::Hidden:
        return CSSValueHidden;
    case BorderStyle::Inset:
        return CSSValueInset;
    case BorderStyle::Groove:
        return CSSValueGroove;
    case BorderStyle::Ridge:
        return CSSValueRidge;
    case BorderStyle::Outset:
        return CSSValueOutset;
    case BorderStyle::Dotted:
        return CSSValueDotted;
    case BorderStyle::Dashed:
        return CSSValueDashed;
    case BorderStyle::Solid:
        return CSSValueSolid;
    case BorderStyle::Double:
        return CSSValueDouble;
    }
}

template<> constexpr BorderStyle fromCSSValueID(CSSValueID valueID)
{
    if (valueID == CSSValueAuto) // Valid for CSS outline-style
        return BorderStyle::Dotted;
    return static_cast<BorderStyle>(valueID - CSSValueNone);
}

template<> constexpr OutlineIsAuto fromCSSValueID(CSSValueID valueID)
{
    if (valueID == CSSValueAuto)
        return OutlineIsAuto::On;
    return OutlineIsAuto::Off;
}

constexpr CSSValueID toCSSValueID(CompositeOperator e, CSSPropertyID propertyID)
{
    if (propertyID == CSSPropertyMaskComposite) {
        switch (e) {
        case CompositeOperator::SourceOver:
            return CSSValueAdd;
        case CompositeOperator::SourceIn:
            return CSSValueIntersect;
        case CompositeOperator::SourceOut:
            return CSSValueSubtract;
        case CompositeOperator::XOR:
            return CSSValueExclude;
        default:
            break;
        }
    } else {
        switch (e) {
        case CompositeOperator::Clear:
            return CSSValueClear;
        case CompositeOperator::Copy:
            return CSSValueCopy;
        case CompositeOperator::SourceOver:
            return CSSValueSourceOver;
        case CompositeOperator::SourceIn:
            return CSSValueSourceIn;
        case CompositeOperator::SourceOut:
            return CSSValueSourceOut;
        case CompositeOperator::SourceAtop:
            return CSSValueSourceAtop;
        case CompositeOperator::DestinationOver:
            return CSSValueDestinationOver;
        case CompositeOperator::DestinationIn:
            return CSSValueDestinationIn;
        case CompositeOperator::DestinationOut:
            return CSSValueDestinationOut;
        case CompositeOperator::DestinationAtop:
            return CSSValueDestinationAtop;
        case CompositeOperator::XOR:
            return CSSValueXor;
        case CompositeOperator::PlusDarker:
            return CSSValuePlusDarker;
        case CompositeOperator::PlusLighter:
            return CSSValuePlusLighter;
        case CompositeOperator::Difference:
            break;
        }
    }
    RELEASE_ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
}

template<> constexpr CompositeOperator fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueClear:
        return CompositeOperator::Clear;
    case CSSValueCopy:
        return CompositeOperator::Copy;
    case CSSValueSourceOver:
    case CSSValueAdd:
        return CompositeOperator::SourceOver;
    case CSSValueSourceIn:
    case CSSValueIntersect:
        return CompositeOperator::SourceIn;
    case CSSValueSourceOut:
    case CSSValueSubtract:
        return CompositeOperator::SourceOut;
    case CSSValueSourceAtop:
        return CompositeOperator::SourceAtop;
    case CSSValueDestinationOver:
        return CompositeOperator::DestinationOver;
    case CSSValueDestinationIn:
        return CompositeOperator::DestinationIn;
    case CSSValueDestinationOut:
        return CompositeOperator::DestinationOut;
    case CSSValueDestinationAtop:
        return CompositeOperator::DestinationAtop;
    case CSSValueXor:
    case CSSValueExclude:
        return CompositeOperator::XOR;
    case CSSValuePlusDarker:
        return CompositeOperator::PlusDarker;
    case CSSValuePlusLighter:
        return CompositeOperator::PlusLighter;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return CompositeOperator::Clear;
}

constexpr CSSValueID toCSSValueID(StyleAppearance e)
{
    switch (e) {
    case StyleAppearance::None:
        return CSSValueNone;
    case StyleAppearance::Auto:
        return CSSValueAuto;
    case StyleAppearance::Checkbox:
        return CSSValueCheckbox;
    case StyleAppearance::Radio:
        return CSSValueRadio;
    case StyleAppearance::PushButton:
        return CSSValuePushButton;
    case StyleAppearance::SquareButton:
        return CSSValueSquareButton;
    case StyleAppearance::Button:
        return CSSValueButton;
    case StyleAppearance::DefaultButton:
        return CSSValueDefaultButton;
    case StyleAppearance::Listbox:
        return CSSValueListbox;
    case StyleAppearance::Menulist:
        return CSSValueMenulist;
    case StyleAppearance::MenulistButton:
        return CSSValueMenulistButton;
    case StyleAppearance::Meter:
        return CSSValueMeter;
    case StyleAppearance::ProgressBar:
        return CSSValueProgressBar;
    case StyleAppearance::SliderHorizontal:
        return CSSValueSliderHorizontal;
    case StyleAppearance::SliderVertical:
        return CSSValueSliderVertical;
    case StyleAppearance::SearchField:
        return CSSValueSearchfield;
    case StyleAppearance::TextField:
        return CSSValueTextfield;
    case StyleAppearance::TextArea:
        return CSSValueTextarea;
#if ENABLE(ATTACHMENT_ELEMENT)
    case StyleAppearance::Attachment:
        return CSSValueAttachment;
    case StyleAppearance::BorderlessAttachment:
        return CSSValueBorderlessAttachment;
#endif
#if ENABLE(APPLE_PAY)
    case StyleAppearance::ApplePayButton:
        return CSSValueApplePayButton;
#endif
    case StyleAppearance::CapsLockIndicator:
#if ENABLE(INPUT_TYPE_COLOR)
    case StyleAppearance::ColorWell:
#endif
#if ENABLE(SERVICE_CONTROLS)
    case StyleAppearance::ImageControlsButton:
#endif
    case StyleAppearance::InnerSpinButton:
#if ENABLE(DATALIST_ELEMENT)
    case StyleAppearance::ListButton:
#endif
    case StyleAppearance::SearchFieldDecoration:
    case StyleAppearance::SearchFieldResultsDecoration:
    case StyleAppearance::SearchFieldResultsButton:
    case StyleAppearance::SearchFieldCancelButton:
    case StyleAppearance::SliderThumbHorizontal:
    case StyleAppearance::SliderThumbVertical:
        ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
        return CSSValueNone;
    }
}

template<> constexpr StyleAppearance fromCSSValueID(CSSValueID valueID)
{
    if (valueID == CSSValueNone)
        return StyleAppearance::None;

    if (valueID == CSSValueAuto)
        return StyleAppearance::Auto;

    return StyleAppearance(valueID - CSSValueCheckbox + static_cast<unsigned>(StyleAppearance::Checkbox));
}

#define TYPE BackfaceVisibility
#define FOR_EACH(CASE) CASE(Visible) CASE(Hidden)
DEFINE_TO_FROM_CSS_VALUE_ID_FUNCTIONS
#undef TYPE
#undef FOR_EACH

constexpr CSSValueID toCSSValueID(FillAttachment e)
{
    switch (e) {
    case FillAttachment::ScrollBackground:
        return CSSValueScroll;
    case FillAttachment::LocalBackground:
        return CSSValueLocal;
    case FillAttachment::FixedBackground:
        return CSSValueFixed;
    }
}

template<> constexpr FillAttachment fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueScroll:
        return FillAttachment::ScrollBackground;
    case CSSValueLocal:
        return FillAttachment::LocalBackground;
    case CSSValueFixed:
        return FillAttachment::FixedBackground;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return FillAttachment::ScrollBackground;
}

constexpr CSSValueID toCSSValueID(FillBox e)
{
    switch (e) {
    case FillBox::Border:
        return CSSValueBorderBox;
    case FillBox::Padding:
        return CSSValuePaddingBox;
    case FillBox::Content:
        return CSSValueContentBox;
    case FillBox::Text:
        return CSSValueText;
    case FillBox::NoClip:
        return CSSValueNoClip;
    }
}

template<> constexpr FillBox fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueBorder:
    case CSSValueBorderBox:
        return FillBox::Border;
    case CSSValuePadding:
    case CSSValuePaddingBox:
        return FillBox::Padding;
    case CSSValueContent:
    case CSSValueContentBox:
        return FillBox::Content;
    case CSSValueText:
    case CSSValueWebkitText:
        return FillBox::Text;
    case CSSValueNoClip:
        return FillBox::NoClip;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return FillBox::Border;
}

#define TYPE FillRepeat
#define FOR_EACH(CASE) CASE(Repeat) CASE(NoRepeat) CASE(Round) CASE(Space)
DEFINE_TO_FROM_CSS_VALUE_ID_FUNCTIONS
#undef TYPE
#undef FOR_EACH

#define TYPE BoxPack
#define FOR_EACH(CASE) CASE(Start) CASE(Center) CASE(End) CASE(Justify)
DEFINE_TO_FROM_CSS_VALUE_ID_FUNCTIONS
#undef TYPE
#undef FOR_EACH

#define TYPE BoxAlignment
#define FOR_EACH(CASE) CASE(Stretch) CASE(Start) CASE(Center) CASE(End) CASE(Baseline)
DEFINE_TO_FROM_CSS_VALUE_ID_FUNCTIONS
#undef TYPE
#undef FOR_EACH

#if ENABLE(CSS_BOX_DECORATION_BREAK)

#define TYPE BoxDecorationBreak
#define FOR_EACH(CASE) CASE(Slice) CASE(Clone)
DEFINE_TO_FROM_CSS_VALUE_ID_FUNCTIONS
#undef TYPE
#undef FOR_EACH

#endif

#define TYPE Edge
#define FOR_EACH(CASE) CASE(Top) CASE(Right) CASE(Bottom) CASE(Left)
DEFINE_TO_FROM_CSS_VALUE_ID_FUNCTIONS
#undef TYPE
#undef FOR_EACH

#define TYPE BoxSizing
#define FOR_EACH(CASE) CASE(BorderBox) CASE(ContentBox)
DEFINE_TO_FROM_CSS_VALUE_ID_FUNCTIONS
#undef TYPE
#undef FOR_EACH

#define TYPE BoxDirection
#define FOR_EACH(CASE) CASE(Normal) CASE(Reverse)
DEFINE_TO_FROM_CSS_VALUE_ID_FUNCTIONS
#undef TYPE
#undef FOR_EACH

#define TYPE BoxLines
#define FOR_EACH(CASE) CASE(Single) CASE(Multiple)
DEFINE_TO_FROM_CSS_VALUE_ID_FUNCTIONS
#undef TYPE
#undef FOR_EACH

#define TYPE BoxOrient
#define FOR_EACH(CASE) CASE(Horizontal) CASE(Vertical)
DEFINE_TO_FROM_CSS_VALUE_ID_FUNCTIONS
#undef TYPE
#undef FOR_EACH

#define TYPE CaptionSide
#define FOR_EACH(CASE) CASE(Left) CASE(Right) CASE(Top) CASE(Bottom)
DEFINE_TO_FROM_CSS_VALUE_ID_FUNCTIONS
#undef TYPE
#undef FOR_EACH

#define TYPE Clear
#define FOR_EACH(CASE) CASE(None) CASE(Left) CASE(Right) CASE(InlineStart) CASE(InlineEnd) CASE(Both)
DEFINE_TO_FROM_CSS_VALUE_ID_FUNCTIONS
#undef TYPE
#undef FOR_EACH

constexpr CSSValueID toCSSValueID(LeadingTrim value)
{
    switch (value) {
    case LeadingTrim::Normal:
        return CSSValueNormal;
    case LeadingTrim::Start:
        return CSSValueStart;
    case LeadingTrim::End:
        return CSSValueEnd;
    case LeadingTrim::Both:
        return CSSValueBoth;
    }
}

template<> constexpr LeadingTrim fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNormal:
        return LeadingTrim::Normal;
    case CSSValueStart:
        return LeadingTrim::Start;
    case CSSValueEnd:
        return LeadingTrim::End;
    case CSSValueBoth:
        return LeadingTrim::Both;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return LeadingTrim::Normal;
}

constexpr CSSValueID toCSSValueID(CursorType e)
{
    switch (e) {
    case CursorType::Auto:
        return CSSValueAuto;
    case CursorType::Default:
        return CSSValueDefault;
    case CursorType::None:
        return CSSValueNone;
    case CursorType::ContextMenu:
        return CSSValueContextMenu;
    case CursorType::Help:
        return CSSValueHelp;
    case CursorType::Pointer:
        return CSSValuePointer;
    case CursorType::Progress:
        return CSSValueProgress;
    case CursorType::Wait:
        return CSSValueWait;
    case CursorType::Cell:
        return CSSValueCell;
    case CursorType::Crosshair:
        return CSSValueCrosshair;
    case CursorType::Text:
        return CSSValueText;
    case CursorType::VerticalText:
        return CSSValueVerticalText;
    case CursorType::Alias:
        return CSSValueAlias;
    case CursorType::Copy:
        return CSSValueCopy;
    case CursorType::Move:
        return CSSValueMove;
    case CursorType::NoDrop:
        return CSSValueNoDrop;
    case CursorType::NotAllowed:
        return CSSValueNotAllowed;
    case CursorType::Grab:
        return CSSValueGrab;
    case CursorType::Grabbing:
        return CSSValueGrabbing;
    case CursorType::EResize:
        return CSSValueEResize;
    case CursorType::NResize:
        return CSSValueNResize;
    case CursorType::NEResize:
        return CSSValueNeResize;
    case CursorType::NWResize:
        return CSSValueNwResize;
    case CursorType::SResize:
        return CSSValueSResize;
    case CursorType::SEResize:
        return CSSValueSeResize;
    case CursorType::SWResize:
        return CSSValueSwResize;
    case CursorType::WResize:
        return CSSValueWResize;
    case CursorType::EWResize:
        return CSSValueEwResize;
    case CursorType::NSResize:
        return CSSValueNsResize;
    case CursorType::NESWResize:
        return CSSValueNeswResize;
    case CursorType::NWSEResize:
        return CSSValueNwseResize;
    case CursorType::ColumnResize:
        return CSSValueColResize;
    case CursorType::RowResize:
        return CSSValueRowResize;
    case CursorType::AllScroll:
        return CSSValueAllScroll;
    case CursorType::ZoomIn:
        return CSSValueZoomIn;
    case CursorType::ZoomOut:
        return CSSValueZoomOut;
    }
}

template<> constexpr CursorType fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueCopy:
        return CursorType::Copy;
    case CSSValueWebkitGrab:
        return CursorType::Grab;
    case CSSValueWebkitGrabbing:
        return CursorType::Grabbing;
    case CSSValueWebkitZoomIn:
        return CursorType::ZoomIn;
    case CSSValueWebkitZoomOut:
        return CursorType::ZoomOut;
    case CSSValueNone:
        return CursorType::None;
    default:
        return static_cast<CursorType>(valueID - CSSValueAuto);
    }
}

#if ENABLE(CURSOR_VISIBILITY)

constexpr CSSValueID toCSSValueID(CursorVisibility e)
{
    switch (e) {
    case CursorVisibility::Auto:
        return CSSValueAuto;
    case CursorVisibility::AutoHide:
        return CSSValueAutoHide;
    }
}

template<> constexpr CursorVisibility fromCSSValueID(CSSValueID valueID)
{
    if (valueID == CSSValueAuto)
        return CursorVisibility::Auto;
    if (valueID == CSSValueAutoHide)
        return CursorVisibility::AutoHide;

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return CursorVisibility::Auto;
}

#endif

constexpr CSSValueID toCSSValueID(DisplayType e)
{
    switch (e) {
    case DisplayType::Inline:
        return CSSValueInline;
    case DisplayType::Block:
        return CSSValueBlock;
    case DisplayType::ListItem:
        return CSSValueListItem;
    case DisplayType::InlineBlock:
        return CSSValueInlineBlock;
    case DisplayType::Table:
        return CSSValueTable;
    case DisplayType::InlineTable:
        return CSSValueInlineTable;
    case DisplayType::TableRowGroup:
        return CSSValueTableRowGroup;
    case DisplayType::TableHeaderGroup:
        return CSSValueTableHeaderGroup;
    case DisplayType::TableFooterGroup:
        return CSSValueTableFooterGroup;
    case DisplayType::TableRow:
        return CSSValueTableRow;
    case DisplayType::TableColumnGroup:
        return CSSValueTableColumnGroup;
    case DisplayType::TableColumn:
        return CSSValueTableColumn;
    case DisplayType::TableCell:
        return CSSValueTableCell;
    case DisplayType::TableCaption:
        return CSSValueTableCaption;
    case DisplayType::Box:
        return CSSValueWebkitBox;
    case DisplayType::InlineBox:
        return CSSValueWebkitInlineBox;
    case DisplayType::Flex:
        return CSSValueFlex;
    case DisplayType::InlineFlex:
        return CSSValueInlineFlex;
    case DisplayType::Grid:
        return CSSValueGrid;
    case DisplayType::InlineGrid:
        return CSSValueInlineGrid;
    case DisplayType::None:
        return CSSValueNone;
    case DisplayType::Contents:
        return CSSValueContents;
    case DisplayType::FlowRoot:
        return CSSValueFlowRoot;
    }
}

template<> constexpr DisplayType fromCSSValueID(CSSValueID valueID)
{
    if (valueID == CSSValueNone)
        return DisplayType::None;

    DisplayType display = static_cast<DisplayType>(valueID - CSSValueInline);
    ASSERT(display >= DisplayType::Inline && display <= DisplayType::None);
    return display;
}

constexpr CSSValueID toCSSValueID(EmptyCell e)
{
    switch (e) {
    case EmptyCell::Show:
        return CSSValueShow;
    case EmptyCell::Hide:
        return CSSValueHide;
    }
}

template<> constexpr EmptyCell fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueShow:
        return EmptyCell::Show;
    case CSSValueHide:
        return EmptyCell::Hide;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return EmptyCell::Show;
}

constexpr CSSValueID toCSSValueID(FlexDirection e)
{
    switch (e) {
    case FlexDirection::Row:
        return CSSValueRow;
    case FlexDirection::RowReverse:
        return CSSValueRowReverse;
    case FlexDirection::Column:
        return CSSValueColumn;
    case FlexDirection::ColumnReverse:
        return CSSValueColumnReverse;
    }
}

template<> constexpr FlexDirection fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueRow:
        return FlexDirection::Row;
    case CSSValueRowReverse:
        return FlexDirection::RowReverse;
    case CSSValueColumn:
        return FlexDirection::Column;
    case CSSValueColumnReverse:
        return FlexDirection::ColumnReverse;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return FlexDirection::Row;
}

constexpr CSSValueID toCSSValueID(FlexWrap e)
{
    switch (e) {
    case FlexWrap::NoWrap:
        return CSSValueNowrap;
    case FlexWrap::Wrap:
        return CSSValueWrap;
    case FlexWrap::Reverse:
        return CSSValueWrapReverse;
    }
}

template<> constexpr FlexWrap fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNowrap:
        return FlexWrap::NoWrap;
    case CSSValueWrap:
        return FlexWrap::Wrap;
    case CSSValueWrapReverse:
        return FlexWrap::Reverse;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return FlexWrap::NoWrap;
}

constexpr CSSValueID toCSSValueID(Float e)
{
    switch (e) {
    case Float::None:
        return CSSValueNone;
    case Float::Left:
        return CSSValueLeft;
    case Float::Right:
        return CSSValueRight;
    case Float::InlineStart:
        return CSSValueInlineStart;
    case Float::InlineEnd:
        return CSSValueInlineEnd;
    }
}

template<> constexpr Float fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueLeft:
        return Float::Left;
    case CSSValueRight:
        return Float::Right;
    case CSSValueInlineStart:
        return Float::InlineStart;
    case CSSValueInlineEnd:
        return Float::InlineEnd;
    case CSSValueNone:
    case CSSValueCenter: // Non-standard CSS value.
        return Float::None;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return Float::None;
}

constexpr CSSValueID toCSSValueID(LineBreak e)
{
    switch (e) {
    case LineBreak::Auto:
        return CSSValueAuto;
    case LineBreak::Loose:
        return CSSValueLoose;
    case LineBreak::Normal:
        return CSSValueNormal;
    case LineBreak::Strict:
        return CSSValueStrict;
    case LineBreak::AfterWhiteSpace:
        return CSSValueAfterWhiteSpace;
    case LineBreak::Anywhere:
        return CSSValueAnywhere;
    }
}

template<> constexpr OptionSet<HangingPunctuation> fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNone:
        return { };
    case CSSValueFirst:
        return HangingPunctuation::First;
    case CSSValueLast:
        return HangingPunctuation::Last;
    case CSSValueAllowEnd:
        return HangingPunctuation::AllowEnd;
    case CSSValueForceEnd:
        return HangingPunctuation::ForceEnd;
    default:
        break;
    }
    
    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return OptionSet<HangingPunctuation> { };
}

template<> constexpr LineBreak fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return LineBreak::Auto;
    case CSSValueLoose:
        return LineBreak::Loose;
    case CSSValueNormal:
        return LineBreak::Normal;
    case CSSValueStrict:
        return LineBreak::Strict;
    case CSSValueAfterWhiteSpace:
        return LineBreak::AfterWhiteSpace;
    case CSSValueAnywhere:
        return LineBreak::Anywhere;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return LineBreak::Auto;
}

constexpr CSSValueID toCSSValueID(ListStylePosition e)
{
    switch (e) {
    case ListStylePosition::Outside:
        return CSSValueOutside;
    case ListStylePosition::Inside:
        return CSSValueInside;
    }
}

template<> constexpr ListStylePosition fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueOutside:
        return ListStylePosition::Outside;
    case CSSValueInside:
        return ListStylePosition::Inside;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return ListStylePosition::Outside;
}

constexpr CSSValueID toCSSValueID(ListStyleType style)
{
    switch (style) {
    case ListStyleType::None:
        return CSSValueNone;
    case ListStyleType::String:
        ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
        return CSSValueInvalid;
    default:
        return static_cast<CSSValueID>(static_cast<int>(CSSValueDisc) + static_cast<uint8_t>(style));
    }
}

template<> constexpr ListStyleType fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNone:
        return ListStyleType::None;
    default:
        return static_cast<ListStyleType>(valueID - CSSValueDisc);
    }
}

constexpr CSSValueID toCSSValueID(MarqueeBehavior e)
{
    switch (e) {
    case MarqueeBehavior::None:
        return CSSValueNone;
    case MarqueeBehavior::Scroll:
        return CSSValueScroll;
    case MarqueeBehavior::Slide:
        return CSSValueSlide;
    case MarqueeBehavior::Alternate:
        return CSSValueAlternate;
    }
}

template<> constexpr MarqueeBehavior fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNone:
        return MarqueeBehavior::None;
    case CSSValueScroll:
        return MarqueeBehavior::Scroll;
    case CSSValueSlide:
        return MarqueeBehavior::Slide;
    case CSSValueAlternate:
        return MarqueeBehavior::Alternate;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return MarqueeBehavior::None;
}

constexpr CSSValueID toCSSValueID(MarqueeDirection direction)
{
    switch (direction) {
    case MarqueeDirection::Forward:
        return CSSValueForwards;
    case MarqueeDirection::Backward:
        return CSSValueBackwards;
    case MarqueeDirection::Auto:
        return CSSValueAuto;
    case MarqueeDirection::Up:
        return CSSValueUp;
    case MarqueeDirection::Down:
        return CSSValueDown;
    case MarqueeDirection::Left:
        return CSSValueLeft;
    case MarqueeDirection::Right:
        return CSSValueRight;
    }
}

template<> constexpr MarqueeDirection fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueForwards:
        return MarqueeDirection::Forward;
    case CSSValueBackwards:
        return MarqueeDirection::Backward;
    case CSSValueAuto:
        return MarqueeDirection::Auto;
    case CSSValueAhead:
    case CSSValueUp: // We don't support vertical languages, so AHEAD just maps to UP.
        return MarqueeDirection::Up;
    case CSSValueReverse:
    case CSSValueDown: // REVERSE just maps to DOWN, since we don't do vertical text.
        return MarqueeDirection::Down;
    case CSSValueLeft:
        return MarqueeDirection::Left;
    case CSSValueRight:
        return MarqueeDirection::Right;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return MarqueeDirection::Auto;
}

constexpr CSSValueID toCSSValueID(NBSPMode e)
{
    switch (e) {
    case NBSPMode::Normal:
        return CSSValueNormal;
    case NBSPMode::Space:
        return CSSValueSpace;
    }
}

template<> constexpr NBSPMode fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueSpace:
        return NBSPMode::Space;
    case CSSValueNormal:
        return NBSPMode::Normal;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return NBSPMode::Normal;
}

constexpr CSSValueID toCSSValueID(Overflow e)
{
    switch (e) {
    case Overflow::Visible:
        return CSSValueVisible;
    case Overflow::Hidden:
        return CSSValueHidden;
    case Overflow::Scroll:
        return CSSValueScroll;
    case Overflow::Auto:
        return CSSValueAuto;
    case Overflow::PagedX:
        return CSSValueWebkitPagedX;
    case Overflow::PagedY:
        return CSSValueWebkitPagedY;
    case Overflow::Clip:
        return CSSValueClip;
    }
}

template<> constexpr Overflow fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueVisible:
        return Overflow::Visible;
    case CSSValueHidden:
        return Overflow::Hidden;
    case CSSValueScroll:
        return Overflow::Scroll;
    case CSSValueOverlay:
    case CSSValueAuto:
        return Overflow::Auto;
    case CSSValueWebkitPagedX:
        return Overflow::PagedX;
    case CSSValueWebkitPagedY:
        return Overflow::PagedY;
    case CSSValueClip:
        return Overflow::Clip;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return Overflow::Visible;
}

constexpr CSSValueID toCSSValueID(OverscrollBehavior behavior)
{
    switch (behavior) {
    case OverscrollBehavior::Contain:
        return CSSValueContain;
    case OverscrollBehavior::None:
        return CSSValueNone;
    case OverscrollBehavior::Auto:
        return CSSValueAuto;
    }
}

template<> constexpr OverscrollBehavior fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueContain:
        return OverscrollBehavior::Contain;
    case CSSValueNone:
        return OverscrollBehavior::None;
    case CSSValueAuto:
        return OverscrollBehavior::Auto;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return OverscrollBehavior::Auto;
}

constexpr CSSValueID toCSSValueID(OverflowAnchor anchor)
{
    switch (anchor) {
    case OverflowAnchor::None:
        return CSSValueNone;
    case OverflowAnchor::Auto:
        return CSSValueAuto;
    }
}

template<> constexpr OverflowAnchor fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNone:
        return OverflowAnchor::None;
    case CSSValueAuto:
        return OverflowAnchor::Auto;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return OverflowAnchor::Auto;
}

constexpr CSSValueID toCSSValueID(BreakBetween e)
{
    switch (e) {
    case BreakBetween::Auto:
        return CSSValueAuto;
    case BreakBetween::Avoid:
        return CSSValueAvoid;
    case BreakBetween::AvoidColumn:
        return CSSValueAvoidColumn;
    case BreakBetween::AvoidPage:
        return CSSValueAvoidPage;
    case BreakBetween::Column:
        return CSSValueColumn;
    case BreakBetween::Page:
        return CSSValuePage;
    case BreakBetween::LeftPage:
        return CSSValueLeft;
    case BreakBetween::RightPage:
        return CSSValueRight;
    case BreakBetween::RectoPage:
        return CSSValueRecto;
    case BreakBetween::VersoPage:
        return CSSValueVerso;
    }
}

template<> constexpr BreakBetween fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return BreakBetween::Auto;
    case CSSValueAvoid:
        return BreakBetween::Avoid;
    case CSSValueAvoidColumn:
        return BreakBetween::AvoidColumn;
    case CSSValueAvoidPage:
        return BreakBetween::AvoidPage;
    case CSSValueColumn:
        return BreakBetween::Column;
    case CSSValuePage:
        return BreakBetween::Page;
    case CSSValueLeft:
        return BreakBetween::LeftPage;
    case CSSValueRight:
        return BreakBetween::RightPage;
    case CSSValueRecto:
        return BreakBetween::RectoPage;
    case CSSValueVerso:
        return BreakBetween::VersoPage;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return BreakBetween::Auto;
}

constexpr CSSValueID toCSSValueID(BreakInside e)
{
    switch (e) {
    case BreakInside::Auto:
        return CSSValueAuto;
    case BreakInside::Avoid:
        return CSSValueAvoid;
    case BreakInside::AvoidColumn:
        return CSSValueAvoidColumn;
    case BreakInside::AvoidPage:
        return CSSValueAvoidPage;
    }
}

template<> constexpr BreakInside fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return BreakInside::Auto;
    case CSSValueAvoid:
        return BreakInside::Avoid;
    case CSSValueAvoidColumn:
        return BreakInside::AvoidColumn;
    case CSSValueAvoidPage:
        return BreakInside::AvoidPage;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return BreakInside::Auto;
}

constexpr CSSValueID toCSSValueID(PositionType e)
{
    switch (e) {
    case PositionType::Static:
        return CSSValueStatic;
    case PositionType::Relative:
        return CSSValueRelative;
    case PositionType::Absolute:
        return CSSValueAbsolute;
    case PositionType::Fixed:
        return CSSValueFixed;
    case PositionType::Sticky:
        return CSSValueSticky;
    }
}

template<> constexpr PositionType fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueStatic:
        return PositionType::Static;
    case CSSValueRelative:
        return PositionType::Relative;
    case CSSValueAbsolute:
        return PositionType::Absolute;
    case CSSValueFixed:
        return PositionType::Fixed;
    case CSSValueSticky:
    case CSSValueWebkitSticky:
        return PositionType::Sticky;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return PositionType::Static;
}

constexpr CSSValueID toCSSValueID(Resize e)
{
    switch (e) {
    case Resize::Both:
        return CSSValueBoth;
    case Resize::Horizontal:
        return CSSValueHorizontal;
    case Resize::Vertical:
        return CSSValueVertical;
    case Resize::Block:
        return CSSValueBlock;
    case Resize::Inline:
        return CSSValueInline;
    case Resize::None:
        return CSSValueNone;
    }
}

template<> constexpr Resize fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueBoth:
        return Resize::Both;
    case CSSValueHorizontal:
        return Resize::Horizontal;
    case CSSValueVertical:
        return Resize::Vertical;
    case CSSValueBlock:
        return Resize::Block;
    case CSSValueInline:
        return Resize::Inline;
    case CSSValueAuto:
        ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT(); // Depends on settings, thus should be handled by the caller.
        return Resize::None;
    case CSSValueNone:
        return Resize::None;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return Resize::None;
}

constexpr CSSValueID toCSSValueID(TableLayoutType e)
{
    switch (e) {
    case TableLayoutType::Auto:
        return CSSValueAuto;
    case TableLayoutType::Fixed:
        return CSSValueFixed;
    }
}

template<> constexpr TableLayoutType fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueFixed:
        return TableLayoutType::Fixed;
    case CSSValueAuto:
        return TableLayoutType::Auto;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TableLayoutType::Auto;
}

constexpr CSSValueID toCSSValueID(TextAlignMode e)
{
    switch (e) {
    case TextAlignMode::Start:
        return CSSValueStart;
    case TextAlignMode::End:
        return CSSValueEnd;
    case TextAlignMode::Left:
        return CSSValueLeft;
    case TextAlignMode::Right:
        return CSSValueRight;
    case TextAlignMode::Center:
        return CSSValueCenter;
    case TextAlignMode::Justify:
        return CSSValueJustify;
    case TextAlignMode::WebKitLeft:
        return CSSValueWebkitLeft;
    case TextAlignMode::WebKitRight:
        return CSSValueWebkitRight;
    case TextAlignMode::WebKitCenter:
        return CSSValueWebkitCenter;
    }
}

template<> constexpr TextAlignMode fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueWebkitAuto: // Legacy -webkit-auto. Eqiuvalent to start.
    case CSSValueStart:
        return TextAlignMode::Start;
    case CSSValueEnd:
        return TextAlignMode::End;
    default:
        return static_cast<TextAlignMode>(valueID - CSSValueLeft);
    }
}

constexpr CSSValueID toCSSValueID(TextAlignLast e)
{
    switch (e) {
    case TextAlignLast::Start:
        return CSSValueStart;
    case TextAlignLast::End:
        return CSSValueEnd;
    case TextAlignLast::Left:
        return CSSValueLeft;
    case TextAlignLast::Right:
        return CSSValueRight;
    case TextAlignLast::Center:
        return CSSValueCenter;
    case TextAlignLast::Justify:
        return CSSValueJustify;
    case TextAlignLast::Auto:
        return CSSValueAuto;
    }
}

template<> constexpr TextAlignLast fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return TextAlignLast::Auto;
    case CSSValueStart:
        return TextAlignLast::Start;
    case CSSValueEnd:
        return TextAlignLast::End;
    case CSSValueLeft:
        return TextAlignLast::Left;
    case CSSValueRight:
        return TextAlignLast::Right;
    case CSSValueCenter:
        return TextAlignLast::Center;
    case CSSValueJustify:
        return TextAlignLast::Justify;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TextAlignLast::Auto;
}

constexpr CSSValueID toCSSValueID(TextGroupAlign e)
{
    switch (e) {
    case TextGroupAlign::None:
        return CSSValueNone;
    case TextGroupAlign::Start:
        return CSSValueStart;
    case TextGroupAlign::End:
        return CSSValueEnd;
    case TextGroupAlign::Left:
        return CSSValueLeft;
    case TextGroupAlign::Right:
        return CSSValueRight;
    case TextGroupAlign::Center:
        return CSSValueCenter;
    }
}

template<> constexpr TextGroupAlign fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNone:
        return TextGroupAlign::None;
    case CSSValueStart:
        return TextGroupAlign::Start;
    case CSSValueEnd:
        return TextGroupAlign::End;
    case CSSValueLeft:
        return TextGroupAlign::Left;
    case CSSValueRight:
        return TextGroupAlign::Right;
    case CSSValueCenter:
        return TextGroupAlign::Center;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TextGroupAlign::None;
}

constexpr CSSValueID toCSSValueID(TextJustify e)
{
    switch (e) {
    case TextJustify::Auto:
        return CSSValueAuto;
    case TextJustify::None:
        return CSSValueNone;
    case TextJustify::InterWord:
        return CSSValueInterWord;
    case TextJustify::InterCharacter:
        return CSSValueInterCharacter;
    }
}

template<> constexpr TextJustify fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return TextJustify::Auto;
    case CSSValueNone:
        return TextJustify::None;
    case CSSValueInterWord:
        return TextJustify::InterWord;
    case CSSValueInterCharacter:
    case CSSValueDistribute:
        return TextJustify::InterCharacter;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TextJustify::Auto;
}

template<> constexpr OptionSet<TextDecorationLine> fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNone:
        return OptionSet<TextDecorationLine> { };
    case CSSValueUnderline:
        return TextDecorationLine::Underline;
    case CSSValueOverline:
        return TextDecorationLine::Overline;
    case CSSValueLineThrough:
        return TextDecorationLine::LineThrough;
    case CSSValueBlink:
        return TextDecorationLine::Blink;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return OptionSet<TextDecorationLine> { };
}

template<> constexpr TextDecorationStyle fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueSolid:
        return TextDecorationStyle::Solid;
    case CSSValueDouble:
        return TextDecorationStyle::Double;
    case CSSValueDotted:
        return TextDecorationStyle::Dotted;
    case CSSValueDashed:
        return TextDecorationStyle::Dashed;
    case CSSValueWavy:
        return TextDecorationStyle::Wavy;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TextDecorationStyle::Solid;
}

constexpr CSSValueID toCSSValueID(TextUnderlinePosition position)
{
    switch (position) {
    case TextUnderlinePosition::Auto:
        return CSSValueAuto;
    case TextUnderlinePosition::Under:
        return CSSValueUnder;
    case TextUnderlinePosition::FromFont:
        return CSSValueFromFont;
    }

    // FIXME: Implement support for 'under left' and 'under right' values.
}

template<> constexpr TextUnderlinePosition fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return TextUnderlinePosition::Auto;
    case CSSValueUnder:
        return TextUnderlinePosition::Under;
    case CSSValueFromFont:
        return TextUnderlinePosition::FromFont;
    default:
        break;
    }

    // FIXME: Implement support for 'under left' and 'under right' values.
    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TextUnderlinePosition::Auto;
}

constexpr CSSValueID toCSSValueID(TextSecurity e)
{
    switch (e) {
    case TextSecurity::None:
        return CSSValueNone;
    case TextSecurity::Disc:
        return CSSValueDisc;
    case TextSecurity::Circle:
        return CSSValueCircle;
    case TextSecurity::Square:
        return CSSValueSquare;
    }
}

template<> constexpr TextSecurity fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNone:
        return TextSecurity::None;
    case CSSValueDisc:
        return TextSecurity::Disc;
    case CSSValueCircle:
        return TextSecurity::Circle;
    case CSSValueSquare:
        return TextSecurity::Square;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TextSecurity::None;
}

constexpr CSSValueID toCSSValueID(TextDecorationSkipInk e)
{
    switch (e) {
    case TextDecorationSkipInk::None:
        return CSSValueNone;
    case TextDecorationSkipInk::Auto:
        return CSSValueAuto;
    case TextDecorationSkipInk::All:
        return CSSValueAll;
    }
}

template<> constexpr TextDecorationSkipInk fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNone:
        return TextDecorationSkipInk::None;
    case CSSValueAuto:
        return TextDecorationSkipInk::Auto;
    case CSSValueAll:
        return TextDecorationSkipInk::All;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TextDecorationSkipInk::None;
}

constexpr CSSValueID toCSSValueID(TextTransform e)
{
    switch (e) {
    case TextTransform::Capitalize:
        return CSSValueCapitalize;
    case TextTransform::Uppercase:
        return CSSValueUppercase;
    case TextTransform::Lowercase:
        return CSSValueLowercase;
    case TextTransform::None:
        return CSSValueNone;
    }
}

template<> constexpr TextTransform fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueCapitalize:
        return TextTransform::Capitalize;
    case CSSValueUppercase:
        return TextTransform::Uppercase;
    case CSSValueLowercase:
        return TextTransform::Lowercase;
    case CSSValueNone:
        return TextTransform::None;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TextTransform::None;
}

constexpr CSSValueID toCSSValueID(UnicodeBidi e)
{
    switch (e) {
    case UnicodeBidi::Normal:
        return CSSValueNormal;
    case UnicodeBidi::Embed:
        return CSSValueEmbed;
    case UnicodeBidi::Override:
        return CSSValueBidiOverride;
    case UnicodeBidi::Isolate:
        return CSSValueIsolate;
    case UnicodeBidi::IsolateOverride:
        return CSSValueIsolateOverride;
    case UnicodeBidi::Plaintext:
        return CSSValuePlaintext;
    }
}

template<> constexpr UnicodeBidi fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNormal:
        return UnicodeBidi::Normal;
    case CSSValueEmbed:
        return UnicodeBidi::Embed;
    case CSSValueBidiOverride:
        return UnicodeBidi::Override;
    case CSSValueIsolate:
    case CSSValueWebkitIsolate:
        return UnicodeBidi::Isolate;
    case CSSValueIsolateOverride:
    case CSSValueWebkitIsolateOverride:
        return UnicodeBidi::IsolateOverride;
    case CSSValuePlaintext:
    case CSSValueWebkitPlaintext:
        return UnicodeBidi::Plaintext;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return UnicodeBidi::Normal;
}

constexpr CSSValueID toCSSValueID(UserDrag e)
{
    switch (e) {
    case UserDrag::Auto:
        return CSSValueAuto;
    case UserDrag::None:
        return CSSValueNone;
    case UserDrag::Element:
        return CSSValueElement;
    default:
        break;
    }
}

template<> constexpr UserDrag fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return UserDrag::Auto;
    case CSSValueNone:
        return UserDrag::None;
    case CSSValueElement:
        return UserDrag::Element;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return UserDrag::Auto;
}

constexpr CSSValueID toCSSValueID(UserModify e)
{
    switch (e) {
    case UserModify::ReadOnly:
        return CSSValueReadOnly;
    case UserModify::ReadWrite:
        return CSSValueReadWrite;
    case UserModify::ReadWritePlaintextOnly:
        return CSSValueReadWritePlaintextOnly;
    }
}

template<> constexpr UserModify fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueReadOnly:
        return UserModify::ReadOnly;
    case CSSValueReadWrite:
        return UserModify::ReadWrite;
    case CSSValueReadWritePlaintextOnly:
        return UserModify::ReadWritePlaintextOnly;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return UserModify::ReadOnly;
}

constexpr CSSValueID toCSSValueID(UserSelect e)
{
    switch (e) {
    case UserSelect::None:
        return CSSValueNone;
    case UserSelect::Text:
        return CSSValueText;
    case UserSelect::All:
        return CSSValueAll;
    }
}

template<> constexpr UserSelect fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return UserSelect::Text;
    case CSSValueNone:
        return UserSelect::None;
    case CSSValueText:
        return UserSelect::Text;
    case CSSValueAll:
        return UserSelect::All;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return UserSelect::Text;
}

constexpr CSSValueID toCSSValueID(VerticalAlign a)
{
    switch (a) {
    case VerticalAlign::Top:
        return CSSValueTop;
    case VerticalAlign::Bottom:
        return CSSValueBottom;
    case VerticalAlign::Middle:
        return CSSValueMiddle;
    case VerticalAlign::Baseline:
        return CSSValueBaseline;
    case VerticalAlign::TextBottom:
        return CSSValueTextBottom;
    case VerticalAlign::TextTop:
        return CSSValueTextTop;
    case VerticalAlign::Sub:
        return CSSValueSub;
    case VerticalAlign::Super:
        return CSSValueSuper;
    case VerticalAlign::BaselineMiddle:
        return CSSValueWebkitBaselineMiddle;
    case VerticalAlign::Length:
        return CSSValueInvalid;
    }
}

template<> constexpr VerticalAlign fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueTop:
        return VerticalAlign::Top;
    case CSSValueBottom:
        return VerticalAlign::Bottom;
    case CSSValueMiddle:
        return VerticalAlign::Middle;
    case CSSValueBaseline:
        return VerticalAlign::Baseline;
    case CSSValueTextBottom:
        return VerticalAlign::TextBottom;
    case CSSValueTextTop:
        return VerticalAlign::TextTop;
    case CSSValueSub:
        return VerticalAlign::Sub;
    case CSSValueSuper:
        return VerticalAlign::Super;
    case CSSValueWebkitBaselineMiddle:
        return VerticalAlign::BaselineMiddle;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return VerticalAlign::Top;
}

constexpr CSSValueID toCSSValueID(Visibility e)
{
    switch (e) {
    case Visibility::Visible:
        return CSSValueVisible;
    case Visibility::Hidden:
        return CSSValueHidden;
    case Visibility::Collapse:
        return CSSValueCollapse;
    }
}

template<> constexpr Visibility fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueHidden:
        return Visibility::Hidden;
    case CSSValueVisible:
        return Visibility::Visible;
    case CSSValueCollapse:
        return Visibility::Collapse;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return Visibility::Visible;
}

constexpr CSSValueID toCSSValueID(WhiteSpace e)
{
    switch (e) {
    case WhiteSpace::Normal:
        return CSSValueNormal;
    case WhiteSpace::Pre:
        return CSSValuePre;
    case WhiteSpace::PreWrap:
        return CSSValuePreWrap;
    case WhiteSpace::PreLine:
        return CSSValuePreLine;
    case WhiteSpace::NoWrap:
        return CSSValueNowrap;
    case WhiteSpace::BreakSpaces:
        return CSSValueBreakSpaces;
    }
}

template<> constexpr WhiteSpace fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNowrap:
        return WhiteSpace::NoWrap;
    case CSSValuePre:
        return WhiteSpace::Pre;
    case CSSValuePreWrap:
        return WhiteSpace::PreWrap;
    case CSSValuePreLine:
        return WhiteSpace::PreLine;
    case CSSValueNormal:
        return WhiteSpace::Normal;
    case CSSValueBreakSpaces:
        return WhiteSpace::BreakSpaces;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return WhiteSpace::Normal;
}

constexpr CSSValueID toCSSValueID(WordBreak e)
{
    switch (e) {
    case WordBreak::Normal:
        return CSSValueNormal;
    case WordBreak::BreakAll:
        return CSSValueBreakAll;
    case WordBreak::KeepAll:
        return CSSValueKeepAll;
    case WordBreak::BreakWord:
        return CSSValueBreakWord;
    }
}

template<> constexpr WordBreak fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueBreakAll:
        return WordBreak::BreakAll;
    case CSSValueKeepAll:
        return WordBreak::KeepAll;
    case CSSValueBreakWord:
        return WordBreak::BreakWord;
    case CSSValueNormal:
        return WordBreak::Normal;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return WordBreak::Normal;
}

constexpr CSSValueID toCSSValueID(OverflowWrap e)
{
    switch (e) {
    case OverflowWrap::Normal:
        return CSSValueNormal;
    case OverflowWrap::Anywhere:
        return CSSValueAnywhere;
    case OverflowWrap::BreakWord:
        return CSSValueBreakWord;
    }
}

template<> constexpr OverflowWrap fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueBreakWord:
        return OverflowWrap::BreakWord;
    case CSSValueAnywhere:
        return OverflowWrap::Anywhere;
    case CSSValueNormal:
        return OverflowWrap::Normal;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return OverflowWrap::Normal;
}

constexpr CSSValueID toCSSValueID(TextDirection e)
{
    switch (e) {
    case TextDirection::LTR:
        return CSSValueLtr;
    case TextDirection::RTL:
        return CSSValueRtl;
    }
}

template<> constexpr TextDirection fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueLtr:
        return TextDirection::LTR;
    case CSSValueRtl:
        return TextDirection::RTL;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TextDirection::LTR;
}

constexpr CSSValueID toCSSValueID(WritingMode e)
{
    switch (e) {
    case WritingMode::TopToBottom:
        return CSSValueHorizontalTb;
    case WritingMode::RightToLeft:
        return CSSValueVerticalRl;
    case WritingMode::LeftToRight:
        return CSSValueVerticalLr;
    case WritingMode::BottomToTop:
        return CSSValueHorizontalBt;
    }
}

template<> constexpr WritingMode fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueHorizontalTb:
    case CSSValueLr:
    case CSSValueLrTb:
    case CSSValueRl:
    case CSSValueRlTb:
        return WritingMode::TopToBottom;
    case CSSValueVerticalRl:
    case CSSValueTb:
    case CSSValueTbRl:
        return WritingMode::RightToLeft;
    case CSSValueVerticalLr:
        return WritingMode::LeftToRight;
    case CSSValueHorizontalBt:
        return WritingMode::BottomToTop;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return WritingMode::TopToBottom;
}

constexpr CSSValueID toCSSValueID(TextCombine e)
{
    switch (e) {
    case TextCombine::None:
        return CSSValueNone;
    case TextCombine::All:
        return CSSValueAll;
    }
}

template<> constexpr TextCombine fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNone:
        return TextCombine::None;
    case CSSValueAll:
    case CSSValueHorizontal: // -webkit-text-combine only
        return TextCombine::All;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TextCombine::None;
}

constexpr CSSValueID toCSSValueID(RubyPosition position)
{
    switch (position) {
    case RubyPosition::Before:
        return CSSValueBefore;
    case RubyPosition::After:
        return CSSValueAfter;
    case RubyPosition::InterCharacter:
        return CSSValueInterCharacter;
    }
}

template<> constexpr RubyPosition fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueBefore:
        return RubyPosition::Before;
    case CSSValueAfter:
        return RubyPosition::After;
    case CSSValueInterCharacter:
        return RubyPosition::InterCharacter;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return RubyPosition::Before;
}

constexpr CSSValueID toCSSValueID(TextOverflow overflow)
{
    switch (overflow) {
    case TextOverflow::Clip:
        return CSSValueClip;
    case TextOverflow::Ellipsis:
        return CSSValueEllipsis;
    }
}

template<> constexpr TextOverflow fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueClip:
        return TextOverflow::Clip;
    case CSSValueEllipsis:
        return TextOverflow::Ellipsis;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TextOverflow::Clip;
}

constexpr CSSValueID toCSSValueID(TextWrap wrap)
{
    switch (wrap) {
    case TextWrap::Wrap:
        return CSSValueWrap;
    case TextWrap::NoWrap:
        return CSSValueNowrap;
    case TextWrap::Balance:
        return CSSValueBalance;
    case TextWrap::Stable:
        return CSSValueStable;
    case TextWrap::Pretty:
        return CSSValuePretty;
    }
}

template<> constexpr TextWrap fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueWrap:
        return TextWrap::Wrap;
    case CSSValueNowrap:
        return TextWrap::NoWrap;
    case CSSValueBalance:
        return TextWrap::Balance;
    case CSSValueStable:
        return TextWrap::Stable;
    case CSSValuePretty:
        return TextWrap::Pretty;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TextWrap::Wrap;
}

constexpr CSSValueID toCSSValueID(TextEmphasisFill fill)
{
    switch (fill) {
    case TextEmphasisFill::Filled:
        return CSSValueFilled;
    case TextEmphasisFill::Open:
        return CSSValueOpen;
    }
}

template<> constexpr TextEmphasisFill fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueFilled:
        return TextEmphasisFill::Filled;
    case CSSValueOpen:
        return TextEmphasisFill::Open;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TextEmphasisFill::Filled;
}

constexpr CSSValueID toCSSValueID(TextEmphasisMark mark)
{
    switch (mark) {
    case TextEmphasisMark::Dot:
        return CSSValueDot;
    case TextEmphasisMark::Circle:
        return CSSValueCircle;
    case TextEmphasisMark::DoubleCircle:
        return CSSValueDoubleCircle;
    case TextEmphasisMark::Triangle:
        return CSSValueTriangle;
    case TextEmphasisMark::Sesame:
        return CSSValueSesame;
    case TextEmphasisMark::None:
    case TextEmphasisMark::Auto:
    case TextEmphasisMark::Custom:
        ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
        return CSSValueNone;
    }
}

template<> constexpr TextEmphasisMark fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNone:
        return TextEmphasisMark::None;
    case CSSValueDot:
        return TextEmphasisMark::Dot;
    case CSSValueCircle:
        return TextEmphasisMark::Circle;
    case CSSValueDoubleCircle:
        return TextEmphasisMark::DoubleCircle;
    case CSSValueTriangle:
        return TextEmphasisMark::Triangle;
    case CSSValueSesame:
        return TextEmphasisMark::Sesame;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TextEmphasisMark::None;
}

constexpr CSSValueID toCSSValueID(TextOrientation e)
{
    switch (e) {
    case TextOrientation::Sideways:
        return CSSValueSideways;
    case TextOrientation::Mixed:
        return CSSValueMixed;
    case TextOrientation::Upright:
        return CSSValueUpright;
    }
}

template<> constexpr TextOrientation fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueSideways:
        return TextOrientation::Sideways;
    case CSSValueMixed:
        return TextOrientation::Mixed;
    case CSSValueUpright:
        return TextOrientation::Upright;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TextOrientation::Mixed;
}

constexpr CSSValueID toCSSValueID(PointerEvents e)
{
    switch (e) {
    case PointerEvents::None:
        return CSSValueNone;
    case PointerEvents::Stroke:
        return CSSValueStroke;
    case PointerEvents::Fill:
        return CSSValueFill;
    case PointerEvents::Painted:
        return CSSValuePainted;
    case PointerEvents::Visible:
        return CSSValueVisible;
    case PointerEvents::VisibleStroke:
        return CSSValueVisibleStroke;
    case PointerEvents::VisibleFill:
        return CSSValueVisibleFill;
    case PointerEvents::VisiblePainted:
        return CSSValueVisiblePainted;
    case PointerEvents::BoundingBox:
        return CSSValueBoundingBox;
    case PointerEvents::Auto:
        return CSSValueAuto;
    case PointerEvents::All:
        return CSSValueAll;
    }
}

template<> constexpr PointerEvents fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAll:
        return PointerEvents::All;
    case CSSValueAuto:
        return PointerEvents::Auto;
    case CSSValueNone:
        return PointerEvents::None;
    case CSSValueVisiblePainted:
        return PointerEvents::VisiblePainted;
    case CSSValueVisibleFill:
        return PointerEvents::VisibleFill;
    case CSSValueVisibleStroke:
        return PointerEvents::VisibleStroke;
    case CSSValueVisible:
        return PointerEvents::Visible;
    case CSSValuePainted:
        return PointerEvents::Painted;
    case CSSValueFill:
        return PointerEvents::Fill;
    case CSSValueStroke:
        return PointerEvents::Stroke;
    case CSSValueBoundingBox:
        return PointerEvents::BoundingBox;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return PointerEvents::All;
}

constexpr CSSValueID toCSSValueID(Kerning kerning)
{
    switch (kerning) {
    case Kerning::Auto:
        return CSSValueAuto;
    case Kerning::Normal:
        return CSSValueNormal;
    case Kerning::NoShift:
        return CSSValueNone;
    }
}

template<> constexpr Kerning fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return Kerning::Auto;
    case CSSValueNormal:
        return Kerning::Normal;
    case CSSValueNone:
        return Kerning::NoShift;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return Kerning::Auto;
}

constexpr CSSValueID toCSSValueID(ObjectFit fit)
{
    switch (fit) {
    case ObjectFit::Fill:
        return CSSValueFill;
    case ObjectFit::Contain:
        return CSSValueContain;
    case ObjectFit::Cover:
        return CSSValueCover;
    case ObjectFit::None:
        return CSSValueNone;
    case ObjectFit::ScaleDown:
        return CSSValueScaleDown;
    }
}

template<> constexpr ObjectFit fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueFill:
        return ObjectFit::Fill;
    case CSSValueContain:
        return ObjectFit::Contain;
    case CSSValueCover:
        return ObjectFit::Cover;
    case CSSValueNone:
        return ObjectFit::None;
    case CSSValueScaleDown:
        return ObjectFit::ScaleDown;
    default:
        ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
        return ObjectFit::Fill;
    }
}

constexpr CSSValueID toCSSValueID(FontSmoothingMode smoothing)
{
    switch (smoothing) {
    case FontSmoothingMode::AutoSmoothing:
        return CSSValueAuto;
    case FontSmoothingMode::NoSmoothing:
        return CSSValueNone;
    case FontSmoothingMode::Antialiased:
        return CSSValueAntialiased;
    case FontSmoothingMode::SubpixelAntialiased:
        return CSSValueSubpixelAntialiased;
    }
}

template<> constexpr FontSmoothingMode fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return FontSmoothingMode::AutoSmoothing;
    case CSSValueNone:
        return FontSmoothingMode::NoSmoothing;
    case CSSValueAntialiased:
        return FontSmoothingMode::Antialiased;
    case CSSValueSubpixelAntialiased:
        return FontSmoothingMode::SubpixelAntialiased;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return FontSmoothingMode::AutoSmoothing;
}

constexpr CSSValueID toCSSValueID(FontSmallCaps smallCaps)
{
    switch (smallCaps) {
    case FontSmallCaps::Off:
        return CSSValueNormal;
    case FontSmallCaps::On:
        return CSSValueSmallCaps;
    }
}

template<> constexpr FontSmallCaps fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueSmallCaps:
        return FontSmallCaps::On;
    case CSSValueNormal:
        return FontSmallCaps::Off;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return FontSmallCaps::Off;
}

constexpr CSSValueID toCSSValueID(TextRenderingMode e)
{
    switch (e) {
    case TextRenderingMode::AutoTextRendering:
        return CSSValueAuto;
    case TextRenderingMode::OptimizeSpeed:
        return CSSValueOptimizeSpeed;
    case TextRenderingMode::OptimizeLegibility:
        return CSSValueOptimizeLegibility;
    case TextRenderingMode::GeometricPrecision:
        return CSSValueGeometricPrecision;
    }
}

template<> constexpr TextRenderingMode fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return TextRenderingMode::AutoTextRendering;
    case CSSValueOptimizeSpeed:
        return TextRenderingMode::OptimizeSpeed;
    case CSSValueOptimizeLegibility:
        return TextRenderingMode::OptimizeLegibility;
    case CSSValueGeometricPrecision:
        return TextRenderingMode::GeometricPrecision;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TextRenderingMode::AutoTextRendering;
}

constexpr CSSValueID toCSSValueID(Hyphens hyphens)
{
    switch (hyphens) {
    case Hyphens::None:
        return CSSValueNone;
    case Hyphens::Manual:
        return CSSValueManual;
    case Hyphens::Auto:
        return CSSValueAuto;
    }
}

template<> constexpr Hyphens fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNone:
        return Hyphens::None;
    case CSSValueManual:
        return Hyphens::Manual;
    case CSSValueAuto:
        return Hyphens::Auto;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return Hyphens::Auto;
}

constexpr CSSValueID toCSSValueID(LineSnap gridSnap)
{
    switch (gridSnap) {
    case LineSnap::None:
        return CSSValueNone;
    case LineSnap::Baseline:
        return CSSValueBaseline;
    case LineSnap::Contain:
        return CSSValueContain;
    }
}

template<> constexpr LineSnap fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNone:
        return LineSnap::None;
    case CSSValueBaseline:
        return LineSnap::Baseline;
    case CSSValueContain:
        return LineSnap::Contain;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return LineSnap::None;
}

constexpr CSSValueID toCSSValueID(LineAlign lineAlign)
{
    switch (lineAlign) {
    case LineAlign::None:
        return CSSValueNone;
    case LineAlign::Edges:
        return CSSValueEdges;
    }
}

template<> constexpr LineAlign fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNone:
        return LineAlign::None;
    case CSSValueEdges:
        return LineAlign::Edges;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return LineAlign::None;
}

constexpr CSSValueID toCSSValueID(SpeakAs e)
{
    switch (e) {
    case SpeakAs::Normal:
        return CSSValueNormal;
    case SpeakAs::SpellOut:
        return CSSValueSpellOut;
    case SpeakAs::Digits:
        return CSSValueDigits;
    case SpeakAs::LiteralPunctuation:
        return CSSValueLiteralPunctuation;
    case SpeakAs::NoPunctuation:
        return CSSValueNoPunctuation;
    }
}

template<> constexpr Order fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueLogical:
        return Order::Logical;
    case CSSValueVisual:
        return Order::Visual;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return Order::Logical;
}

constexpr CSSValueID toCSSValueID(Order e)
{
    switch (e) {
    case Order::Logical:
        return CSSValueLogical;
    case Order::Visual:
        return CSSValueVisual;
    }
}

template<> constexpr OptionSet<SpeakAs> fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNormal:
        return OptionSet<SpeakAs> { };
    case CSSValueSpellOut:
        return SpeakAs::SpellOut;
    case CSSValueDigits:
        return SpeakAs::Digits;
    case CSSValueLiteralPunctuation:
        return SpeakAs::LiteralPunctuation;
    case CSSValueNoPunctuation:
        return SpeakAs::NoPunctuation;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return OptionSet<SpeakAs> { };
}

constexpr CSSValueID toCSSValueID(BlendMode blendMode)
{
    switch (blendMode) {
    case BlendMode::Normal:
        return CSSValueNormal;
    case BlendMode::Multiply:
        return CSSValueMultiply;
    case BlendMode::Screen:
        return CSSValueScreen;
    case BlendMode::Overlay:
        return CSSValueOverlay;
    case BlendMode::Darken:
        return CSSValueDarken;
    case BlendMode::Lighten:
        return CSSValueLighten;
    case BlendMode::ColorDodge:
        return CSSValueColorDodge;
    case BlendMode::ColorBurn:
        return CSSValueColorBurn;
    case BlendMode::HardLight:
        return CSSValueHardLight;
    case BlendMode::SoftLight:
        return CSSValueSoftLight;
    case BlendMode::Difference:
        return CSSValueDifference;
    case BlendMode::Exclusion:
        return CSSValueExclusion;
    case BlendMode::Hue:
        return CSSValueHue;
    case BlendMode::Saturation:
        return CSSValueSaturation;
    case BlendMode::Color:
        return CSSValueColor;
    case BlendMode::Luminosity:
        return CSSValueLuminosity;
    case BlendMode::PlusDarker:
        return CSSValuePlusDarker;
    case BlendMode::PlusLighter:
        return CSSValuePlusLighter;
    }
}

template<> constexpr BlendMode fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNormal:
        return BlendMode::Normal;
    case CSSValueMultiply:
        return BlendMode::Multiply;
    case CSSValueScreen:
        return BlendMode::Screen;
    case CSSValueOverlay:
        return BlendMode::Overlay;
    case CSSValueDarken:
        return BlendMode::Darken;
    case CSSValueLighten:
        return BlendMode::Lighten;
    case CSSValueColorDodge:
        return BlendMode::ColorDodge;
    case CSSValueColorBurn:
        return BlendMode::ColorBurn;
    case CSSValueHardLight:
        return BlendMode::HardLight;
    case CSSValueSoftLight:
        return BlendMode::SoftLight;
    case CSSValueDifference:
        return BlendMode::Difference;
    case CSSValueExclusion:
        return BlendMode::Exclusion;
    case CSSValueHue:
        return BlendMode::Hue;
    case CSSValueSaturation:
        return BlendMode::Saturation;
    case CSSValueColor:
        return BlendMode::Color;
    case CSSValueLuminosity:
        return BlendMode::Luminosity;
    case CSSValuePlusDarker:
        return BlendMode::PlusDarker;
    case CSSValuePlusLighter:
        return BlendMode::PlusLighter;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return BlendMode::Normal;
}

constexpr CSSValueID toCSSValueID(Isolation isolation)
{
    switch (isolation) {
    case Isolation::Auto:
        return CSSValueAuto;
    case Isolation::Isolate:
        return CSSValueIsolate;
    default:
        ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    }
}

template<> constexpr Isolation fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return Isolation::Auto;
    case CSSValueIsolate:
        return Isolation::Isolate;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return Isolation::Auto;
}

constexpr CSSValueID toCSSValueID(LineCap e)
{
    switch (e) {
    case LineCap::Butt:
        return CSSValueButt;
    case LineCap::Round:
        return CSSValueRound;
    case LineCap::Square:
        return CSSValueSquare;
    }
}

template<> constexpr LineCap fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueButt:
        return LineCap::Butt;
    case CSSValueRound:
        return LineCap::Round;
    case CSSValueSquare:
        return LineCap::Square;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return LineCap::Butt;
}

constexpr CSSValueID toCSSValueID(LineJoin e)
{
    switch (e) {
    case LineJoin::Miter:
        return CSSValueMiter;
    case LineJoin::Round:
        return CSSValueRound;
    case LineJoin::Bevel:
        return CSSValueBevel;
    }
}

template<> constexpr LineJoin fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueMiter:
        return LineJoin::Miter;
    case CSSValueRound:
        return LineJoin::Round;
    case CSSValueBevel:
        return LineJoin::Bevel;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return LineJoin::Miter;
}

constexpr CSSValueID toCSSValueID(WindRule e)
{
    switch (e) {
    case WindRule::NonZero:
        return CSSValueNonzero;
    case WindRule::EvenOdd:
        return CSSValueEvenodd;
    }
}

template<> constexpr WindRule fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNonzero:
        return WindRule::NonZero;
    case CSSValueEvenodd:
        return WindRule::EvenOdd;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return WindRule::NonZero;
}

constexpr CSSValueID toCSSValueID(AlignmentBaseline e)
{
    switch (e) {
    case AlignmentBaseline::Auto:
        return CSSValueAuto;
    case AlignmentBaseline::Baseline:
        return CSSValueBaseline;
    case AlignmentBaseline::BeforeEdge:
        return CSSValueBeforeEdge;
    case AlignmentBaseline::TextBeforeEdge:
        return CSSValueTextBeforeEdge;
    case AlignmentBaseline::Middle:
        return CSSValueMiddle;
    case AlignmentBaseline::Central:
        return CSSValueCentral;
    case AlignmentBaseline::AfterEdge:
        return CSSValueAfterEdge;
    case AlignmentBaseline::TextAfterEdge:
        return CSSValueTextAfterEdge;
    case AlignmentBaseline::Ideographic:
        return CSSValueIdeographic;
    case AlignmentBaseline::Alphabetic:
        return CSSValueAlphabetic;
    case AlignmentBaseline::Hanging:
        return CSSValueHanging;
    case AlignmentBaseline::Mathematical:
        return CSSValueMathematical;
    }
}

template<> constexpr AlignmentBaseline fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return AlignmentBaseline::Auto;
    case CSSValueBaseline:
        return AlignmentBaseline::Baseline;
    case CSSValueBeforeEdge:
        return AlignmentBaseline::BeforeEdge;
    case CSSValueTextBeforeEdge:
        return AlignmentBaseline::TextBeforeEdge;
    case CSSValueMiddle:
        return AlignmentBaseline::Middle;
    case CSSValueCentral:
        return AlignmentBaseline::Central;
    case CSSValueAfterEdge:
        return AlignmentBaseline::AfterEdge;
    case CSSValueTextAfterEdge:
        return AlignmentBaseline::TextAfterEdge;
    case CSSValueIdeographic:
        return AlignmentBaseline::Ideographic;
    case CSSValueAlphabetic:
        return AlignmentBaseline::Alphabetic;
    case CSSValueHanging:
        return AlignmentBaseline::Hanging;
    case CSSValueMathematical:
        return AlignmentBaseline::Mathematical;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return AlignmentBaseline::Auto;
}

constexpr CSSValueID toCSSValueID(BorderCollapse e)
{
    switch (e) {
    case BorderCollapse::Separate:
        return CSSValueSeparate;
    case BorderCollapse::Collapse:
        return CSSValueCollapse;
    }
}

template<> constexpr BorderCollapse fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueSeparate:
        return BorderCollapse::Separate;
    case CSSValueCollapse:
        return BorderCollapse::Collapse;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return BorderCollapse::Separate;
}

constexpr CSSValueID toCSSValueID(ImageRendering imageRendering)
{
    switch (imageRendering) {
    case ImageRendering::Auto:
        return CSSValueAuto;
    case ImageRendering::CrispEdges:
        return CSSValueCrispEdges;
    case ImageRendering::Pixelated:
        return CSSValuePixelated;
    case ImageRendering::OptimizeSpeed:
        return CSSValueOptimizeSpeed;
    case ImageRendering::OptimizeQuality:
        return CSSValueOptimizeQuality;
    }
}

template<> constexpr ImageRendering fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return ImageRendering::Auto;
    case CSSValueWebkitOptimizeContrast:
    case CSSValueCrispEdges:
    case CSSValueWebkitCrispEdges:
        return ImageRendering::CrispEdges;
    case CSSValuePixelated:
        return ImageRendering::Pixelated;
    case CSSValueOptimizeSpeed:
        return ImageRendering::OptimizeSpeed;
    case CSSValueOptimizeQuality:
        return ImageRendering::OptimizeQuality;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return ImageRendering::Auto;
}

constexpr CSSValueID toCSSValueID(InputSecurity e)
{
    switch (e) {
    case InputSecurity::Auto:
        return CSSValueAuto;
    case InputSecurity::None:
        return CSSValueNone;
    }
}

template<> constexpr InputSecurity fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return InputSecurity::Auto;
    case CSSValueNone:
        return InputSecurity::None;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return InputSecurity::Auto;
}

constexpr CSSValueID toCSSValueID(TransformStyle3D e)
{
    switch (e) {
    case TransformStyle3D::Flat:
        return CSSValueFlat;
    case TransformStyle3D::Preserve3D:
        return CSSValuePreserve3d;
#if ENABLE(CSS_TRANSFORM_STYLE_OPTIMIZED_3D)
    case TransformStyle3D::Optimized3D:
        return CSSValueOptimized3d;
#endif
    }
}

template<> constexpr TransformStyle3D fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueFlat:
        return TransformStyle3D::Flat;
    case CSSValuePreserve3d:
        return TransformStyle3D::Preserve3D;
#if ENABLE(CSS_TRANSFORM_STYLE_OPTIMIZED_3D)
    case CSSValueOptimized3d:
        return TransformStyle3D::Optimized3D;
#endif
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TransformStyle3D::Flat;
}

constexpr CSSValueID toCSSValueID(TransformBox box)
{
    switch (box) {
    case TransformBox::StrokeBox:
        return CSSValueStrokeBox;
    case TransformBox::ContentBox:
        return CSSValueContentBox;
    case TransformBox::BorderBox:
        return CSSValueBorderBox;
    case TransformBox::FillBox:
        return CSSValueFillBox;
    case TransformBox::ViewBox:
        return CSSValueViewBox;
    }
}

template<> constexpr TransformBox fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueStrokeBox:
        return TransformBox::StrokeBox;
    case CSSValueContentBox:
        return TransformBox::ContentBox;
    case CSSValueBorderBox:
        return TransformBox::BorderBox;
    case CSSValueFillBox:
        return TransformBox::FillBox;
    case CSSValueViewBox:
        return TransformBox::ViewBox;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TransformBox::BorderBox;
}

constexpr CSSValueID toCSSValueID(ColumnAxis e)
{
    switch (e) {
    case ColumnAxis::Horizontal:
        return CSSValueHorizontal;
    case ColumnAxis::Vertical:
        return CSSValueVertical;
    case ColumnAxis::Auto:
        return CSSValueAuto;
    }
}

template<> constexpr ColumnAxis fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueHorizontal:
        return ColumnAxis::Horizontal;
    case CSSValueVertical:
        return ColumnAxis::Vertical;
    case CSSValueAuto:
        return ColumnAxis::Auto;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return ColumnAxis::Auto;
}

constexpr CSSValueID toCSSValueID(ColumnProgression e)
{
    switch (e) {
    case ColumnProgression::Normal:
        return CSSValueNormal;
    case ColumnProgression::Reverse:
        return CSSValueReverse;
    }
}

template<> constexpr ColumnProgression fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNormal:
        return ColumnProgression::Normal;
    case CSSValueReverse:
        return ColumnProgression::Reverse;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return ColumnProgression::Normal;
}

enum LengthConversion {
    AnyConversion = ~0,
    FixedIntegerConversion = 1 << 0,
    FixedFloatConversion = 1 << 1,
    AutoConversion = 1 << 2,
    PercentConversion = 1 << 3,
    CalculatedConversion = 1 << 4
};

inline bool CSSPrimitiveValue::convertingToLengthRequiresNonNullStyle(int lengthConversion) const
{
    // This matches the implementation in CSSPrimitiveValue::computeLengthDouble().
    //
    // FIXME: We should probably make CSSPrimitiveValue::computeLengthDouble and
    // CSSPrimitiveValue::computeNonCalcLengthDouble (which has the style assertion)
    // return std::optional<double> instead of having this check here.
    switch (primitiveUnitType()) {
    case CSSUnitType::CSS_EMS:
    case CSSUnitType::CSS_EXS:
    case CSSUnitType::CSS_CHS:
    case CSSUnitType::CSS_IC:
    case CSSUnitType::CSS_LHS:
        return lengthConversion & (FixedIntegerConversion | FixedFloatConversion);
    case CSSUnitType::CSS_CALC:
        return m_value.calc->convertingToLengthRequiresNonNullStyle(lengthConversion);
    default:
        return false;
    }
}

template<int supported> Length CSSPrimitiveValue::convertToLength(const CSSToLengthConversionData& conversionData) const
{
    if (convertingToLengthRequiresNonNullStyle(supported) && !conversionData.style())
        return Length(LengthType::Undefined);
    if ((supported & FixedIntegerConversion) && isLength())
        return computeLength<Length>(conversionData);
    if ((supported & FixedFloatConversion) && isLength())
        return Length(computeLength<double>(conversionData), LengthType::Fixed);
    if ((supported & PercentConversion) && isPercentage())
        return Length(doubleValue(), LengthType::Percent);
    if ((supported & AutoConversion) && valueID() == CSSValueAuto)
        return Length(LengthType::Auto);
    if ((supported & CalculatedConversion) && isCalculated())
        return Length(cssCalcValue()->createCalculationValue(conversionData));
    return Length(LengthType::Undefined);
}

constexpr CSSValueID toCSSValueID(BufferedRendering e)
{
    switch (e) {
    case BufferedRendering::Auto:
        return CSSValueAuto;
    case BufferedRendering::Dynamic:
        return CSSValueDynamic;
    case BufferedRendering::Static:
        return CSSValueStatic;
    }
}

template<> constexpr BufferedRendering fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return BufferedRendering::Auto;
    case CSSValueDynamic:
        return BufferedRendering::Dynamic;
    case CSSValueStatic:
        return BufferedRendering::Static;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return BufferedRendering::Auto;
}

constexpr CSSValueID toCSSValueID(ColorInterpolation e)
{
    switch (e) {
    case ColorInterpolation::Auto:
        return CSSValueAuto;
    case ColorInterpolation::SRGB:
        return CSSValueSRGB;
    case ColorInterpolation::LinearRGB:
        return CSSValueLinearRGB;
    }
}

template<> constexpr ColorInterpolation fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueSRGB:
        return ColorInterpolation::SRGB;
    case CSSValueLinearRGB:
        return ColorInterpolation::LinearRGB;
    case CSSValueAuto:
        return ColorInterpolation::Auto;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return ColorInterpolation::Auto;
}

constexpr CSSValueID toCSSValueID(ColorRendering e)
{
    switch (e) {
    case ColorRendering::Auto:
        return CSSValueAuto;
    case ColorRendering::OptimizeSpeed:
        return CSSValueOptimizeSpeed;
    case ColorRendering::OptimizeQuality:
        return CSSValueOptimizeQuality;
    }
}

template<> constexpr ColorRendering fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueOptimizeSpeed:
        return ColorRendering::OptimizeSpeed;
    case CSSValueOptimizeQuality:
        return ColorRendering::OptimizeQuality;
    case CSSValueAuto:
        return ColorRendering::Auto;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return ColorRendering::Auto;
}

constexpr CSSValueID toCSSValueID(DominantBaseline e)
{
    switch (e) {
    case DominantBaseline::Auto:
        return CSSValueAuto;
    case DominantBaseline::UseScript:
        return CSSValueUseScript;
    case DominantBaseline::NoChange:
        return CSSValueNoChange;
    case DominantBaseline::ResetSize:
        return CSSValueResetSize;
    case DominantBaseline::Central:
        return CSSValueCentral;
    case DominantBaseline::Middle:
        return CSSValueMiddle;
    case DominantBaseline::TextBeforeEdge:
        return CSSValueTextBeforeEdge;
    case DominantBaseline::TextAfterEdge:
        return CSSValueTextAfterEdge;
    case DominantBaseline::Ideographic:
        return CSSValueIdeographic;
    case DominantBaseline::Alphabetic:
        return CSSValueAlphabetic;
    case DominantBaseline::Hanging:
        return CSSValueHanging;
    case DominantBaseline::Mathematical:
        return CSSValueMathematical;
    }
}

template<> constexpr DominantBaseline fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return DominantBaseline::Auto;
    case CSSValueUseScript:
        return DominantBaseline::UseScript;
    case CSSValueNoChange:
        return DominantBaseline::NoChange;
    case CSSValueResetSize:
        return DominantBaseline::ResetSize;
    case CSSValueIdeographic:
        return DominantBaseline::Ideographic;
    case CSSValueAlphabetic:
        return DominantBaseline::Alphabetic;
    case CSSValueHanging:
        return DominantBaseline::Hanging;
    case CSSValueMathematical:
        return DominantBaseline::Mathematical;
    case CSSValueCentral:
        return DominantBaseline::Central;
    case CSSValueMiddle:
        return DominantBaseline::Middle;
    case CSSValueTextAfterEdge:
        return DominantBaseline::TextAfterEdge;
    case CSSValueTextBeforeEdge:
        return DominantBaseline::TextBeforeEdge;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return DominantBaseline::Auto;
}

constexpr CSSValueID toCSSValueID(ShapeRendering e)
{
    switch (e) {
    case ShapeRendering::Auto:
        return CSSValueAuto;
    case ShapeRendering::OptimizeSpeed:
        return CSSValueOptimizeSpeed;
    case ShapeRendering::CrispEdges:
        return CSSValueCrispedges;
    case ShapeRendering::GeometricPrecision:
        return CSSValueGeometricPrecision;
    }
}

template<> constexpr ShapeRendering fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return ShapeRendering::Auto;
    case CSSValueOptimizeSpeed:
        return ShapeRendering::OptimizeSpeed;
    case CSSValueCrispedges:
        return ShapeRendering::CrispEdges;
    case CSSValueGeometricPrecision:
        return ShapeRendering::GeometricPrecision;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return ShapeRendering::Auto;
}

constexpr CSSValueID toCSSValueID(TextAnchor e)
{
    switch (e) {
    case TextAnchor::Start:
        return CSSValueStart;
    case TextAnchor::Middle:
        return CSSValueMiddle;
    case TextAnchor::End:
        return CSSValueEnd;
    }
}

template<> constexpr TextAnchor fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueStart:
        return TextAnchor::Start;
    case CSSValueMiddle:
        return TextAnchor::Middle;
    case CSSValueEnd:
        return TextAnchor::End;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TextAnchor::Start;
}

#if 0

constexpr CSSValueID toCSSValueID(const Color& color)
    : CSSValue(PrimitiveClass)
{
    setPrimitiveUnitType(CSSUnitType::CSS_RGBCOLOR);
    m_value.color = new Color(color);
}

#endif

constexpr CSSValueID toCSSValueID(VectorEffect e)
{
    switch (e) {
    case VectorEffect::None:
        return CSSValueNone;
    case VectorEffect::NonScalingStroke:
        return CSSValueNonScalingStroke;
    }
}

template<> constexpr VectorEffect fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNone:
        return VectorEffect::None;
    case CSSValueNonScalingStroke:
        return VectorEffect::NonScalingStroke;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return VectorEffect::None;
}

constexpr CSSValueID toCSSValueID(MaskType e)
{
    switch (e) {
    case MaskType::Luminance:
        return CSSValueLuminance;
    case MaskType::Alpha:
        return CSSValueAlpha;
    }
}

template<> constexpr MaskType fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueLuminance:
        return MaskType::Luminance;
    case CSSValueAlpha:
        return MaskType::Alpha;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return MaskType::Luminance;
}

constexpr CSSValueID toCSSValueID(CSSBoxType cssBox)
{
    switch (cssBox) {
    case CSSBoxType::MarginBox:
        return CSSValueMarginBox;
    case CSSBoxType::BorderBox:
        return CSSValueBorderBox;
    case CSSBoxType::PaddingBox:
        return CSSValuePaddingBox;
    case CSSBoxType::ContentBox:
        return CSSValueContentBox;
    case CSSBoxType::FillBox:
        return CSSValueFillBox;
    case CSSBoxType::StrokeBox:
        return CSSValueStrokeBox;
    case CSSBoxType::ViewBox:
        return CSSValueViewBox;
    case CSSBoxType::BoxMissing:
        ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
        return CSSValueNone;
    }
}

template<> constexpr CSSBoxType fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueMarginBox:
        return CSSBoxType::MarginBox;
    case CSSValueBorderBox:
        return CSSBoxType::BorderBox;
    case CSSValuePaddingBox:
        return CSSBoxType::PaddingBox;
    case CSSValueContentBox:
        return CSSBoxType::ContentBox;
    // The following are used in an SVG context.
    case CSSValueFillBox:
        return CSSBoxType::FillBox;
    case CSSValueStrokeBox:
        return CSSBoxType::StrokeBox;
    case CSSValueViewBox:
        return CSSBoxType::ViewBox;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return CSSBoxType::BoxMissing;
}

constexpr CSSValueID toCSSValueID(ItemPosition itemPosition)
{
    switch (itemPosition) {
    case ItemPosition::Legacy:
        return CSSValueLegacy;
    case ItemPosition::Auto:
        return CSSValueAuto;
    case ItemPosition::Normal:
        return CSSValueNormal;
    case ItemPosition::Stretch:
        return CSSValueStretch;
    case ItemPosition::Baseline:
        return CSSValueBaseline;
    case ItemPosition::LastBaseline:
        return CSSValueLastBaseline;
    case ItemPosition::Center:
        return CSSValueCenter;
    case ItemPosition::Start:
        return CSSValueStart;
    case ItemPosition::End:
        return CSSValueEnd;
    case ItemPosition::SelfStart:
        return CSSValueSelfStart;
    case ItemPosition::SelfEnd:
        return CSSValueSelfEnd;
    case ItemPosition::FlexStart:
        return CSSValueFlexStart;
    case ItemPosition::FlexEnd:
        return CSSValueFlexEnd;
    case ItemPosition::Left:
        return CSSValueLeft;
    case ItemPosition::Right:
        return CSSValueRight;
    }
}

template<> constexpr ItemPosition fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueLegacy:
        return ItemPosition::Legacy;
    case CSSValueAuto:
        return ItemPosition::Auto;
    case CSSValueNormal:
        return ItemPosition::Normal;
    case CSSValueStretch:
        return ItemPosition::Stretch;
    case CSSValueBaseline:
        return ItemPosition::Baseline;
    case CSSValueFirstBaseline:
        return ItemPosition::Baseline;
    case CSSValueLastBaseline:
        return ItemPosition::LastBaseline;
    case CSSValueCenter:
        return ItemPosition::Center;
    case CSSValueStart:
        return ItemPosition::Start;
    case CSSValueEnd:
        return ItemPosition::End;
    case CSSValueSelfStart:
        return ItemPosition::SelfStart;
    case CSSValueSelfEnd:
        return ItemPosition::SelfEnd;
    case CSSValueFlexStart:
        return ItemPosition::FlexStart;
    case CSSValueFlexEnd:
        return ItemPosition::FlexEnd;
    case CSSValueLeft:
        return ItemPosition::Left;
    case CSSValueRight:
        return ItemPosition::Right;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return ItemPosition::Auto;
}

constexpr CSSValueID toCSSValueID(OverflowAlignment overflowAlignment)
{
    switch (overflowAlignment) {
    case OverflowAlignment::Default:
        return CSSValueDefault;
    case OverflowAlignment::Unsafe:
        return CSSValueUnsafe;
    case OverflowAlignment::Safe:
        return CSSValueSafe;
    }
}

template<> constexpr OverflowAlignment fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueUnsafe:
        return OverflowAlignment::Unsafe;
    case CSSValueSafe:
        return OverflowAlignment::Safe;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return OverflowAlignment::Unsafe;
}

constexpr CSSValueID toCSSValueID(ContentPosition contentPosition)
{
    switch (contentPosition) {
    case ContentPosition::Normal:
        return CSSValueNormal;
    case ContentPosition::Baseline:
        return CSSValueBaseline;
    case ContentPosition::LastBaseline:
        return CSSValueLastBaseline;
    case ContentPosition::Center:
        return CSSValueCenter;
    case ContentPosition::Start:
        return CSSValueStart;
    case ContentPosition::End:
        return CSSValueEnd;
    case ContentPosition::FlexStart:
        return CSSValueFlexStart;
    case ContentPosition::FlexEnd:
        return CSSValueFlexEnd;
    case ContentPosition::Left:
        return CSSValueLeft;
    case ContentPosition::Right:
        return CSSValueRight;
    }
}

template<> constexpr ContentPosition fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNormal:
        return ContentPosition::Normal;
    case CSSValueBaseline:
        return ContentPosition::Baseline;
    case CSSValueFirstBaseline:
        return ContentPosition::Baseline;
    case CSSValueLastBaseline:
        return ContentPosition::LastBaseline;
    case CSSValueCenter:
        return ContentPosition::Center;
    case CSSValueStart:
        return ContentPosition::Start;
    case CSSValueEnd:
        return ContentPosition::End;
    case CSSValueFlexStart:
        return ContentPosition::FlexStart;
    case CSSValueFlexEnd:
        return ContentPosition::FlexEnd;
    case CSSValueLeft:
        return ContentPosition::Left;
    case CSSValueRight:
        return ContentPosition::Right;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return ContentPosition::Normal;
}

constexpr CSSValueID toCSSValueID(ContentDistribution contentDistribution)
{
    switch (contentDistribution) {
    case ContentDistribution::Default:
        return CSSValueDefault;
    case ContentDistribution::SpaceBetween:
        return CSSValueSpaceBetween;
    case ContentDistribution::SpaceAround:
        return CSSValueSpaceAround;
    case ContentDistribution::SpaceEvenly:
        return CSSValueSpaceEvenly;
    case ContentDistribution::Stretch:
        return CSSValueStretch;
    }
}

template<> constexpr ContentDistribution fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueSpaceBetween:
        return ContentDistribution::SpaceBetween;
    case CSSValueSpaceAround:
        return ContentDistribution::SpaceAround;
    case CSSValueSpaceEvenly:
        return ContentDistribution::SpaceEvenly;
    case CSSValueStretch:
        return ContentDistribution::Stretch;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return ContentDistribution::Stretch;
}

template<> constexpr TextZoom fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNormal:
        return TextZoom::Normal;
    case CSSValueReset:
        return TextZoom::Reset;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TextZoom::Normal;
}

constexpr CSSValueID toCSSValueID(TextZoom textZoom)
{
    switch (textZoom) {
    case TextZoom::Normal:
        return CSSValueNormal;
    case TextZoom::Reset:
        return CSSValueReset;
    }
}

constexpr CSSValueID toCSSValueID(TouchAction touchAction)
{
    switch (touchAction) {
    case TouchAction::Auto:
        return CSSValueAuto;
    case TouchAction::Manipulation:
        return CSSValueManipulation;
    case TouchAction::None:
        return CSSValueNone;
    case TouchAction::PanX:
        return CSSValuePanX;
    case TouchAction::PanY:
        return CSSValuePanY;
    case TouchAction::PinchZoom:
        return CSSValuePinchZoom;
    }
}

template<> constexpr OptionSet<TouchAction> fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return TouchAction::Auto;
    case CSSValueManipulation:
        return TouchAction::Manipulation;
    case CSSValueNone:
        return TouchAction::None;
    case CSSValuePanX:
        return TouchAction::PanX;
    case CSSValuePanY:
        return TouchAction::PanY;
    case CSSValuePinchZoom:
        return TouchAction::PinchZoom;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TouchAction::Auto;
}

constexpr CSSValueID toCSSValueID(ScrollSnapStrictness strictness)
{
    switch (strictness) {
    case ScrollSnapStrictness::None:
        return CSSValueNone;
    case ScrollSnapStrictness::Proximity:
        return CSSValueProximity;
    case ScrollSnapStrictness::Mandatory:
        return CSSValueMandatory;
    }
}

template<> constexpr ScrollSnapStrictness fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNone:
        return ScrollSnapStrictness::None;
    case CSSValueProximity:
        return ScrollSnapStrictness::Proximity;
    case CSSValueMandatory:
        return ScrollSnapStrictness::Mandatory;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return ScrollSnapStrictness::None;
}

constexpr CSSValueID toCSSValueID(ScrollSnapAxis axis)
{
    switch (axis) {
    case ScrollSnapAxis::XAxis:
        return CSSValueX;
    case ScrollSnapAxis::YAxis:
        return CSSValueY;
    case ScrollSnapAxis::Block:
        return CSSValueBlock;
    case ScrollSnapAxis::Inline:
        return CSSValueInline;
    case ScrollSnapAxis::Both:
        return CSSValueBoth;
    }
}

template<> constexpr ScrollSnapAxis fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueX:
        return ScrollSnapAxis::XAxis;
    case CSSValueY:
        return ScrollSnapAxis::YAxis;
    case CSSValueBlock:
        return ScrollSnapAxis::Block;
    case CSSValueInline:
        return ScrollSnapAxis::Inline;
    case CSSValueBoth:
        return ScrollSnapAxis::Both;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return ScrollSnapAxis::Both;
}

constexpr CSSValueID toCSSValueID(ScrollSnapAxisAlignType type)
{
    switch (type) {
    case ScrollSnapAxisAlignType::None:
        return CSSValueNone;
    case ScrollSnapAxisAlignType::Start:
        return CSSValueStart;
    case ScrollSnapAxisAlignType::Center:
        return CSSValueCenter;
    case ScrollSnapAxisAlignType::End:
        return CSSValueEnd;
    }
}

template<> constexpr ScrollSnapAxisAlignType fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNone:
        return ScrollSnapAxisAlignType::None;
    case CSSValueStart:
        return ScrollSnapAxisAlignType::Start;
    case CSSValueCenter:
        return ScrollSnapAxisAlignType::Center;
    case CSSValueEnd:
        return ScrollSnapAxisAlignType::End;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return ScrollSnapAxisAlignType::None;
}

constexpr CSSValueID toCSSValueID(ScrollSnapStop snapStop)
{
    switch (snapStop) {
    case ScrollSnapStop::Normal:
        return CSSValueNormal;
    case ScrollSnapStop::Always:
        return CSSValueAlways;
    }
}

template<> constexpr ScrollSnapStop fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNormal:
        return ScrollSnapStop::Normal;
    case CSSValueAlways:
        return ScrollSnapStop::Always;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return ScrollSnapStop::Normal;
}

constexpr CSSValueID toCSSValueID(TextEdgeType textEdgeType)
{
    switch (textEdgeType) {
    case TextEdgeType::Leading:
        return CSSValueLeading;
    case TextEdgeType::Text:
        return CSSValueText;
    case TextEdgeType::CapHeight:
        return CSSValueCap;
    case TextEdgeType::ExHeight:
        return CSSValueEx;
    case TextEdgeType::Alphabetic:
        return CSSValueAlphabetic;
    case TextEdgeType::CJKIdeographic:
        return CSSValueIdeographic;
    case TextEdgeType::CJKIdeographicInk:
        return CSSValueIdeographicInk;
    }
}

template<> constexpr TextEdgeType fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueLeading:
        return TextEdgeType::Leading;
    case CSSValueText:
        return TextEdgeType::Text;
    case CSSValueCap:
        return TextEdgeType::CapHeight;
    case CSSValueEx:
        return TextEdgeType::ExHeight;
    case CSSValueAlphabetic:
        return TextEdgeType::Alphabetic;
    case CSSValueIdeographic:
        return TextEdgeType::CJKIdeographic;
    case CSSValueIdeographicInk:
        return TextEdgeType::CJKIdeographicInk;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return TextEdgeType::Leading;
}

#if ENABLE(APPLE_PAY)

constexpr CSSValueID toCSSValueID(ApplePayButtonStyle e)
{
    switch (e) {
    case ApplePayButtonStyle::White:
        return CSSValueWhite;
    case ApplePayButtonStyle::WhiteOutline:
        return CSSValueWhiteOutline;
    case ApplePayButtonStyle::Black:
        return CSSValueBlack;
    }
}

template<> constexpr ApplePayButtonStyle fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueWhite:
        return ApplePayButtonStyle::White;
    case CSSValueWhiteOutline:
        return ApplePayButtonStyle::WhiteOutline;
    case CSSValueBlack:
        return ApplePayButtonStyle::Black;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return ApplePayButtonStyle::Black;
}

constexpr CSSValueID toCSSValueID(ApplePayButtonType e)
{
    switch (e) {
    case ApplePayButtonType::Plain:
        return CSSValuePlain;
    case ApplePayButtonType::Buy:
        return CSSValueBuy;
    case ApplePayButtonType::SetUp:
        return CSSValueSetUp;
    case ApplePayButtonType::Donate:
        return CSSValueDonate;
    case ApplePayButtonType::CheckOut:
        return CSSValueCheckOut;
    case ApplePayButtonType::Book:
        return CSSValueBook;
    case ApplePayButtonType::Subscribe:
        return CSSValueSubscribe;
#if ENABLE(APPLE_PAY_NEW_BUTTON_TYPES)
    case ApplePayButtonType::Reload:
        return CSSValueReload;
    case ApplePayButtonType::AddMoney:
        return CSSValueAddMoney;
    case ApplePayButtonType::TopUp:
        return CSSValueTopUp;
    case ApplePayButtonType::Order:
        return CSSValueOrder;
    case ApplePayButtonType::Rent:
        return CSSValueRent;
    case ApplePayButtonType::Support:
        return CSSValueSupport;
    case ApplePayButtonType::Contribute:
        return CSSValueContribute;
    case ApplePayButtonType::Tip:
        return CSSValueTip;
#endif
    }
}

template<> constexpr ApplePayButtonType fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValuePlain:
        return ApplePayButtonType::Plain;
    case CSSValueBuy:
        return ApplePayButtonType::Buy;
    case CSSValueSetUp:
        return ApplePayButtonType::SetUp;
    case CSSValueDonate:
        return ApplePayButtonType::Donate;
    case CSSValueCheckOut:
        return ApplePayButtonType::CheckOut;
    case CSSValueBook:
        return ApplePayButtonType::Book;
    case CSSValueSubscribe:
        return ApplePayButtonType::Subscribe;
#if ENABLE(APPLE_PAY_NEW_BUTTON_TYPES)
    case CSSValueReload:
        return ApplePayButtonType::Reload;
    case CSSValueAddMoney:
        return ApplePayButtonType::AddMoney;
    case CSSValueTopUp:
        return ApplePayButtonType::TopUp;
    case CSSValueOrder:
        return ApplePayButtonType::Order;
    case CSSValueRent:
        return ApplePayButtonType::Rent;
    case CSSValueSupport:
        return ApplePayButtonType::Support;
    case CSSValueContribute:
        return ApplePayButtonType::Contribute;
    case CSSValueTip:
        return ApplePayButtonType::Tip;
#endif
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return ApplePayButtonType::Plain;
}

#endif

constexpr CSSValueID toCSSValueID(FontVariantPosition position)
{
    switch (position) {
    case FontVariantPosition::Normal:
        return CSSValueNormal;
    case FontVariantPosition::Subscript:
        return CSSValueSub;
    case FontVariantPosition::Superscript:
        return CSSValueSuper;
    }
}

template<> constexpr FontVariantPosition fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNormal:
        return FontVariantPosition::Normal;
    case CSSValueSub:
        return FontVariantPosition::Subscript;
    case CSSValueSuper:
        return FontVariantPosition::Superscript;
    default:
        ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
        return FontVariantPosition::Normal;
    }
}

constexpr CSSValueID toCSSValueID(FontVariantCaps caps)
{
    switch (caps) {
    case FontVariantCaps::Normal:
        return CSSValueNormal;
    case FontVariantCaps::Small:
        return CSSValueSmallCaps;
    case FontVariantCaps::AllSmall:
        return CSSValueAllSmallCaps;
    case FontVariantCaps::Petite:
        return CSSValuePetiteCaps;
    case FontVariantCaps::AllPetite:
        return CSSValueAllPetiteCaps;
    case FontVariantCaps::Unicase:
        return CSSValueUnicase;
    case FontVariantCaps::Titling:
        return CSSValueTitlingCaps;
    }
}

template<> constexpr FontVariantCaps fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNormal:
        return FontVariantCaps::Normal;
    case CSSValueSmallCaps:
        return FontVariantCaps::Small;
    case CSSValueAllSmallCaps:
        return FontVariantCaps::AllSmall;
    case CSSValuePetiteCaps:
        return FontVariantCaps::Petite;
    case CSSValueAllPetiteCaps:
        return FontVariantCaps::AllPetite;
    case CSSValueUnicase:
        return FontVariantCaps::Unicase;
    case CSSValueTitlingCaps:
        return FontVariantCaps::Titling;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return FontVariantCaps::Normal;
}

constexpr CSSValueID toCSSValueID(FontOpticalSizing sizing)
{
    switch (sizing) {
    case FontOpticalSizing::Enabled:
        return CSSValueAuto;
    case FontOpticalSizing::Disabled:
        return CSSValueNone;
    }
}

template<> constexpr FontOpticalSizing fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return FontOpticalSizing::Enabled;
    case CSSValueNone:
        return FontOpticalSizing::Disabled;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return FontOpticalSizing::Enabled;
}

constexpr CSSValueID toCSSValueID(FontSynthesisLonghandValue value)
{
    switch (value) {
    case FontSynthesisLonghandValue::Auto:
        return CSSValueAuto;
    case FontSynthesisLonghandValue::None:
        return CSSValueNone;
    }
}

template<> constexpr FontSynthesisLonghandValue fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return FontSynthesisLonghandValue::Auto;
    case CSSValueNone:
        return FontSynthesisLonghandValue::None;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return FontSynthesisLonghandValue::Auto;
}

constexpr CSSValueID toCSSValueID(FontLoadingBehavior behavior)
{
    switch (behavior) {
    case FontLoadingBehavior::Auto:
        return CSSValueAuto;
    case FontLoadingBehavior::Block:
        return CSSValueBlock;
    case FontLoadingBehavior::Swap:
        return CSSValueSwap;
    case FontLoadingBehavior::Fallback:
        return CSSValueFallback;
    case FontLoadingBehavior::Optional:
        return CSSValueOptional;
    }
}

template<> constexpr FontLoadingBehavior fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueAuto:
        return FontLoadingBehavior::Auto;
    case CSSValueBlock:
        return FontLoadingBehavior::Block;
    case CSSValueSwap:
        return FontLoadingBehavior::Swap;
    case CSSValueFallback:
        return FontLoadingBehavior::Fallback;
    case CSSValueOptional:
        return FontLoadingBehavior::Optional;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return FontLoadingBehavior::Auto;
}

constexpr CSSValueID toCSSValueID(MathStyle mathStyle)
{
    switch (mathStyle) {
    case MathStyle::Normal:
        return CSSValueNormal;
    case MathStyle::Compact:
        return CSSValueCompact;
    }
}

template<> constexpr MathStyle fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNormal:
        return MathStyle::Normal;
    case CSSValueCompact:
        return MathStyle::Compact;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return MathStyle::Normal;
}

constexpr CSSValueID toCSSValueID(ContainerType containerType)
{
    switch (containerType) {
    case ContainerType::Normal:
        return CSSValueNormal;
    case ContainerType::Size:
        return CSSValueSize;
    case ContainerType::InlineSize:
        return CSSValueInlineSize;
    }
}

template<> constexpr ContainerType fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueNormal:
        return ContainerType::Normal;
    case CSSValueSize:
        return ContainerType::Size;
    case CSSValueInlineSize:
        return ContainerType::InlineSize;
    default:
        ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
        return ContainerType::Normal;
    }
}

constexpr CSSValueID toCSSValueID(ContentVisibility contentVisibility)
{
    switch (contentVisibility) {
    case ContentVisibility::Visible:
        return CSSValueVisible;
    case ContentVisibility::Hidden:
        return CSSValueHidden;
    case ContentVisibility::Auto:
        return CSSValueAuto;
    }
}

template<> constexpr ContentVisibility fromCSSValueID(CSSValueID valueID)
{
    switch (valueID) {
    case CSSValueVisible:
        return ContentVisibility::Visible;
    case CSSValueHidden:
        return ContentVisibility::Hidden;
    case CSSValueAuto:
        return ContentVisibility::Auto;
    default:
        break;
    }

    ASSERT_NOT_REACHED_UNDER_CONSTEXPR_CONTEXT();
    return ContentVisibility::Visible;
}

#undef EMIT_TO_CSS_SWITCH_CASE
#undef EMIT_FROM_CSS_SWITCH_CASE
#undef DEFINE_TO_FROM_CSS_VALUE_ID_FUNCTIONS

}
