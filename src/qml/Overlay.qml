import QtQuick

// Single shared window for both the target-outline preview and the center
// direction indicator, replacing what used to be two independent
// QQuickWindows. Stacking order between two separate top-level windows
// turned out not to be reliably controllable from C++: neither an explicit
// raise() nor avoiding redundant show() calls on the outline kept the
// indicator reliably above it — and once the outline's own Blur effect
// started blurring whatever's behind it, a wrongly-stacked outline meant
// it was blurring the indicator's already-painted pixels into mush before
// drawing its tinted fill on top.
//
// Stacking between two *Items in one scene* has none of that ambiguity:
// whichever is declared later always paints on top, full stop, with no
// window-manager/compositor timing involved at all. Indicator is declared
// after Outline here for exactly that reason — it must always read as "in
// front of" the outline preview, never behind it.
//
// Outline fills this window entirely (its own target rect is a sub-region
// within that, in its own local coordinates — see Outline.qml). Indicator
// is instead positioned as an explicit sub-rect (x/y/width/height, plain
// QQuickItem properties) within this same window, pushed from
// SirkelEffect::repositionIndicator() — translating its desired absolute
// screen position into this window's local coordinate space, the same way
// Outline's own target rect already is.
Item {
    anchors.fill: parent

    Outline {
        objectName: "outlinePart"
        anchors.fill: parent
    }

    Indicator {
        objectName: "indicatorPart"
    }
}
