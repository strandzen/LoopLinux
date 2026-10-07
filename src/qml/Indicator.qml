import QtQuick

// Center direction indicator: a static ring (square frame at cornerRadius 0,
// full circle at cornerRadius >= width/2 — see
// LoopLiteConfig::IndicatorCornerRadius) with a single marker that slides
// *along the ring's own boundary curve* to the currently selected
// direction — not a straight-line (chord-cutting) tween between two points,
// and not rotating around the center. Sized to the ring's thickness so it
// always sits pixel-fit inside it (Todo.md's "Other Notes" #8). Lives in
// its own small, fully transparent QQuickWindow (see LoopLiteEffect) so the
// shape's rounded/square edges don't show a black background. All style
// properties are pushed from LoopLiteEffect::applyIndicatorStyle().
Item {
    id: root
    anchors.fill: parent

    property string direction: "none"
    property real cornerRadius: 110
    property int ringWidth: 12
    property bool showText: true
    property color highlightColor: "#3daee9"

    readonly property real r: Math.min(cornerRadius, width / 2, height / 2)
    readonly property real inset: ringWidth / 2
    // The border is drawn on the ring's centerline, inset from its outer
    // edge by `inset` — offsetting a rounded corner inward by a distance
    // shrinks its own radius by that same distance (standard rounded-rect
    // offset geometry), not the outer radius again. Using `r` for the arc
    // (as the first version of this did) gave the path's endpoints and its
    // radius two different corners' worth of geometry, so the actual curve
    // Qt drew for them came out smaller/flatter than the ring's real curve
    // — the "pulled toward center" look.
    readonly property real rc: Math.max(0, r - inset)

    // The 8 directions sit at exactly 1/8 intervals of the path's arc
    // length, regardless of cornerRadius: walking from one waypoint to the
    // next always covers (one straight edge's half) + (one corner arc's
    // half), and by symmetry every one of the 8 gaps has that same length.
    // So no per-direction arc-length bookkeeping is needed — just index/8.
    readonly property var directionOrder: ["top", "topRight", "right", "bottomRight", "bottom", "bottomLeft", "left", "topLeft"]

    function progressForDirection(dir) {
        const index = directionOrder.indexOf(dir);
        return index >= 0 ? index / 8 : 0;
    }

    readonly property var directionLabels: ({
        "top": "Top",
        "bottom": "Bottom",
        "left": "Left",
        "right": "Right",
        "topRight": "Top Right",
        "topLeft": "Top Left",
        "bottomRight": "Bottom Right",
        "bottomLeft": "Bottom Left",
    })

    function labelForDirection(dir) {
        return directionLabels[dir] || "";
    }

    // Animated via unwrappedProgress rather than driving pathInterpolator's
    // progress directly, so the transition can always take the *shorter*
    // way around the ring (e.g. topLeft -> top goes forward through 0/1,
    // not backward through right/bottom/left) the same way
    // RotationAnimation.Shortest would for an angle — plain number
    // animation has no such wraparound awareness on its own.
    property real unwrappedProgress: 0

    onDirectionChanged: {
        if (direction === "none") {
            return;
        }
        const target = progressForDirection(direction);
        const current = ((unwrappedProgress % 1) + 1) % 1;
        let delta = target - current;
        if (delta > 0.5) {
            delta -= 1;
        } else if (delta < -0.5) {
            delta += 1;
        }
        unwrappedProgress += delta;
    }

    Behavior on unwrappedProgress {
        NumberAnimation { duration: 260; easing.type: Easing.OutCubic }
    }

    // Static base ring — same shape/color always, never swaps pieces.
    Rectangle {
        anchors.fill: parent
        radius: root.r
        color: "transparent"
        border.width: root.ringWidth
        border.color: "#662e3440"
    }

    // The ring's boundary centerline, traced once clockwise starting at the
    // top-center — this is the path the marker actually slides along.
    PathInterpolator {
        id: pathPos
        progress: ((root.unwrappedProgress % 1) + 1) % 1
        path: Path {
            startX: root.width / 2
            startY: root.inset
            PathLine { x: root.width - root.r; y: root.inset }
            PathArc { x: root.width - root.inset; y: root.r; radiusX: root.rc; radiusY: root.rc; direction: PathArc.Clockwise }
            PathLine { x: root.width - root.inset; y: root.height - root.r }
            PathArc { x: root.width - root.r; y: root.height - root.inset; radiusX: root.rc; radiusY: root.rc; direction: PathArc.Clockwise }
            PathLine { x: root.r; y: root.height - root.inset }
            PathArc { x: root.inset; y: root.height - root.r; radiusX: root.rc; radiusY: root.rc; direction: PathArc.Clockwise }
            PathLine { x: root.inset; y: root.r }
            PathArc { x: root.r; y: root.inset; radiusX: root.rc; radiusY: root.rc; direction: PathArc.Clockwise }
            PathLine { x: root.width / 2; y: root.inset }
        }
    }

    // The sliding marker — its own layer, separate from the base ring.
    Rectangle {
        width: root.ringWidth
        height: root.ringWidth
        radius: width / 2
        color: root.highlightColor
        visible: root.direction !== "none"
        x: pathPos.x - width / 2
        y: pathPos.y - height / 2
    }

    Text {
        anchors.centerIn: parent
        color: "white"
        font.pixelSize: 16
        text: root.labelForDirection(root.direction)
        visible: root.showText && root.direction !== "none"
    }
}
