package prefuse.demos.applets;

import java.awt.Color;
import java.awt.Dimension;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JComponent;
import javax.swing.JSplitPane;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import prefuse.Constants;
import prefuse.Display;
import prefuse.Visualization;
import prefuse.action.ActionList;
import prefuse.action.RepaintAction;
import prefuse.action.assignment.ColorAction;
import prefuse.action.assignment.DataColorAction;
import prefuse.action.assignment.DataShapeAction;
import prefuse.action.filter.GraphDistanceFilter;
import prefuse.action.layout.graph.ForceDirectedLayout;
import prefuse.activity.Activity;
import prefuse.controls.DragControl;
import prefuse.controls.FocusControl;
import prefuse.controls.NeighborHighlightControl;
import prefuse.controls.PanControl;
import prefuse.controls.WheelZoomControl;
import prefuse.controls.ZoomControl;
import prefuse.controls.ZoomToFitControl;
import prefuse.data.Graph;
import prefuse.data.Tuple;
import prefuse.data.event.TupleSetListener;
import prefuse.data.io.GraphMLReader;
import prefuse.data.tuple.TupleSet;
import prefuse.render.DefaultRendererFactory;
import prefuse.render.AbstractShapeRenderer;
import prefuse.render.Renderer;
import prefuse.render.RendererFactory;
import prefuse.render.LabelRenderer;
import prefuse.render.ShapeRenderer;
import prefuse.util.ColorLib;
import prefuse.util.GraphLib;
import prefuse.util.PrefuseLib;
import prefuse.util.force.ForceSimulator;
import prefuse.util.ui.JForcePanel;
import prefuse.util.ui.JPrefuseApplet;
import prefuse.util.ui.JValueSlider;
import prefuse.util.ui.UILib;
import prefuse.visual.NodeItem;
import prefuse.visual.VisualGraph;
import prefuse.visual.VisualItem;

/**
 * @author <a href="http://jheer.org">jeffrey heer</a>
 */
public class GraphView extends JPrefuseApplet {

    private static final String graph = "graph";
    private static final String nodes = "graph.nodes";
    private static final String edges = "graph.edges";
    
    public void init() {
        UILib.setPlatformLookAndFeel();
        JComponent graphview = demo("/output.graphml", "name");
        this.getContentPane().add(graphview);
    }

    public static JComponent demo(String datafile, String label) {
        Graph g = null;
        if ( datafile == null ) {
            g = GraphLib.getGrid(128,128);
        } else {
            try {
                g = new GraphMLReader().readGraph(datafile);
            } catch ( Exception e ) {
                e.printStackTrace();
                System.exit(1);
            }
        }
        return demo(g, label);
    }
    
    public static JComponent demo(Graph g, String label) {

        // create a new, empty visualization for our data
        final Visualization vis = new Visualization();
        VisualGraph vg = vis.addGraph(graph, g);
        vis.setValue(edges, null, VisualItem.INTERACTIVE, Boolean.FALSE);
        int[] palette = new int[] {
                ColorLib.rgb(77, 175, 74),
                ColorLib.rgb(55, 126, 184),
                ColorLib.rgb(228, 26, 28),
                ColorLib.rgb(152, 78, 163),
                ColorLib.rgb(255, 127, 0)
        	};

        int[] shape_palette = new int[] {
        		Constants.SHAPE_ELLIPSE,
        		Constants.SHAPE_HEXAGON,
        		Constants.SHAPE_RECTANGLE
        };
        
        TupleSet focusGroup = vis.getGroup(Visualization.FOCUS_ITEMS); 
        focusGroup.addTupleSetListener(new TupleSetListener() {
            public void tupleSetChanged(TupleSet ts, Tuple[] add, Tuple[] rem)
            {
                for ( int i=0; i<rem.length; ++i )
                    ((VisualItem)rem[i]).setFixed(false);
                for ( int i=0; i<add.length; ++i ) {
                    ((VisualItem)add[i]).setFixed(false);
                    ((VisualItem)add[i]).setFixed(true);
                }
                vis.run("draw");
            }
        });
        
        // set up the renderers
        
/*      ShapeRenderer s = new ShapeRenderer();
        s.rectangle(0, 0, 30, 30);
        vis.setRendererFactory(new DefaultRendererFactory(s));*/
        
        //LabelRenderer tr = new LabelRenderer(label);
        //tr.setRoundedCorner(8, 8);
        //DefaultRendererFactory d = new DefaultRendererFactory(tr);
        //vis.setRendererFactory(d);
        
        MyRenderer tr = new MyRenderer(label); // changed from labelrenderer
        DefaultRendererFactory d = new DefaultRendererFactory(tr);
        vis.setRendererFactory(d); 
        
        //vis.setRendererFactory(new DefaultRendererFactory(tr));
        //AbstractShapeRenderer sr = new ShapeRenderer();
        
        // -- set up the actions ----------------------------------------------
        
        int maxhops = 1024, hops = 1024;
        final GraphDistanceFilter filter = new GraphDistanceFilter(graph, hops);

        ActionList draw = new ActionList();
        draw.add(filter);        
        
        //draw.add(new ColorAction(nodes, VisualItem.FILLCOLOR, ColorLib.rgb(180,180,255)));
        draw.add(new ColorAction(nodes, VisualItem.STROKECOLOR, 0));
        draw.add(new ColorAction(nodes, VisualItem.TEXTCOLOR, ColorLib.rgb(0,0,0)));
        draw.add(new ColorAction(edges, VisualItem.FILLCOLOR, ColorLib.gray(100)));
        draw.add(new ColorAction(edges, VisualItem.STROKECOLOR, ColorLib.gray(170)));
        //draw.add(new DataShapeAction(nodes,"type", shape_palette));
        
        DataColorAction fill = new DataColorAction("graph.nodes", "type",
        	    Constants.NOMINAL, VisualItem.FILLCOLOR, palette);
        	// use black for node text
    	ColorAction text = new ColorAction("graph.nodes",
    	    VisualItem.TEXTCOLOR, ColorLib.gray(0));
    	// use light grey for edges
    	ColorAction edges = new ColorAction("graph.edges",
    	    VisualItem.STROKECOLOR, ColorLib.gray(200));

    	// create an action list containing all color assignments
    	ActionList color = new ActionList();
    	color.add(fill);
    	color.add(text);
    	color.add(edges);
    	
//        ColorAction fill = new ColorAction(nodes, 
//                VisualItem.FILLCOLOR, ColorLib.rgb(200,200,255));
        fill.add("_fixed", ColorLib.rgb(255,100,100));
        fill.add("_highlight", ColorLib.rgb(255,200,125));
        
        ForceDirectedLayout fdl = new ForceDirectedLayout(graph);
        ForceSimulator fsim = fdl.getForceSimulator();
        fsim.getForces()[0].setParameter(0, -2f);
        
        ActionList animate = new ActionList(Activity.INFINITY);

        animate.add(fdl);
        animate.add(fill);
        animate.add(new RepaintAction());
        
        // finally, we register our ActionList with the Visualization.
        // we can later execute our Actions by invoking a method on our
        // Visualization, using the name we've chosen below.
        vis.putAction("draw", draw);
        vis.putAction("layout", animate);
        vis.runAfter("draw", "layout");
        vis.putAction("color", color);
        //vis.putAction("layout", layout);

        
        
        // --------------------------------------------------------------------
        // STEP 4: set up a display to show the visualization
        
        Display display = new Display(vis);
        display.setSize(2000,2000);
        display.setForeground(Color.GRAY);
        display.setBackground(Color.WHITE);
        
        // main display controls
        display.addControlListener(new FocusControl(1));
        display.addControlListener(new DragControl());
        display.addControlListener(new PanControl());
        display.addControlListener(new ZoomControl());
        display.addControlListener(new WheelZoomControl());
        display.addControlListener(new ZoomToFitControl());
        display.addControlListener(new NeighborHighlightControl());
        
        display.setForeground(Color.GRAY);
        display.setBackground(Color.WHITE);
        
        // --------------------------------------------------------------------        
        // STEP 5: launching the visualization
        
        // create a panel for editing force values
        final JForcePanel fpanel = new JForcePanel(fsim);
        
        final JValueSlider slider = new JValueSlider("Distance", 0, maxhops, hops);
        slider.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent e) {
                filter.setDistance(slider.getValue().intValue());
                vis.run("draw");
            }
        });
        slider.setBackground(Color.WHITE);
        slider.setPreferredSize(new Dimension(300,30));
        slider.setMaximumSize(new Dimension(300,30));
        
        Box cf = new Box(BoxLayout.Y_AXIS);
        cf.add(slider);
        cf.setBorder(BorderFactory.createTitledBorder("Connectivity Filter"));
        fpanel.add(cf);
        
        fpanel.add(Box.createVerticalGlue());
        
        // create a new JSplitPane to present the interface
        JSplitPane split = new JSplitPane();
        split.setLeftComponent(display);
        split.setRightComponent(fpanel);
        split.setOneTouchExpandable(true);
        split.setContinuousLayout(false);
        split.setDividerLocation(530);
        split.setDividerLocation(800);
        
        
        // position and fix the default focus node
        NodeItem focus = (NodeItem)vg.getNode(0);
        PrefuseLib.setX(focus, null, 400);
        PrefuseLib.setY(focus, null, 250);
        focusGroup.setTuple(focus);

        // now we run our action list and return
        return split;
    }
    
} // end of class GraphView
