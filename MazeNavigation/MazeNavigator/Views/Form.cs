using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using Maze_generator.Models;
using static System.Net.Mime.MediaTypeNames;

namespace Maze_generator.Views
{
    public sealed partial class Form : System.Windows.Forms.Form
    {
        private Maze _maze;
        private Route aStarRoute;
        private Route gReedyRoute;
        private bool aStarDone;
        private bool gReedyDone;
        private List<Route.pos_t> aStarCpy;
        private List<Route.pos_t> gReedyCpy;

        public Form()
        {
            InitializeComponent();

            const int initialSize = 15;
            _maze = new Maze(new Size(initialSize, initialSize), 0);
            numericSize.Value = initialSize;
            //aStarRouteCount = 0;
            aStarCpy = new List<Route.pos_t>();
            gReedyCpy = new List<Route.pos_t>();

            aStarDone = false;
            gReedyDone = false;

            Refresh();
        }

        private void panel1_Paint(object sender, PaintEventArgs e)
        {
            var g = e.Graphics;
            var pen = new Pen(Color.Black, 1);

            var cellWidth = (float) panel1.Width/_maze.Size.Width;
            var cellHeight = (float) panel1.Height/_maze.Size.Height;

            // Draw begin and end
            var brush = new SolidBrush(Color.Red);
            g.FillRectangle(brush, 0, 0, cellWidth, cellHeight);
            brush.Color = Color.DodgerBlue;
            g.FillRectangle(brush, panel1.Width - cellWidth, panel1.Height - cellHeight, cellWidth, cellHeight);

            foreach (var edge in _maze.Edges)
            {
                if (Math.Abs(edge.Cell1 - edge.Cell2) > 1)
                {
                    // Draw a horizontal line
                    var x = (float) Math.Max(edge.Cell1, edge.Cell2)%_maze.Size.Width*cellWidth;
                    var y = (float) Math.Floor((double) Math.Min(edge.Cell1, edge.Cell2)/_maze.Size.Width + 1)*
                            cellHeight;
                    g.DrawLine(pen, x, y, x + cellWidth, y);
                }
                else
                {
                    // Draw a vertical line
                    var x = (float) Math.Max(edge.Cell1, edge.Cell2)%_maze.Size.Width*cellWidth;
                    var y = (float) Math.Floor((double) Math.Min(edge.Cell1, edge.Cell2)/_maze.Size.Width)*cellHeight;
                    g.DrawLine(pen, x, y, x, y + cellHeight);
                }
            }

            if (aStarRoute != null)
            {
                //var penRoute = new Pen(Color.Green, 1);
                List<Route.pos_t> aStar = aStarRoute.getRoute();
                for (int i = 0; i < aStar.Count; i++)
                {
                    //float mark_x = (aStar.ElementAt(i).pos.X / 100.0F) * panel1.Width;
                    //float mark_y = (aStar.ElementAt(i).pos.Y / 100.0F) * panel1.Height;
                    // window pos = (route pos / 100) * window size
                    //g.DrawEllipse(penRoute, mark_x - 5.0F, mark_y - 5.0F, 5.0F, 5.0F);
                    PointF mark = new PointF();
                    mark = calcRoutePoint(aStar.ElementAt(i));
                    // window pos = (route pos / 100) * window size
                    //g.DrawEllipse(pen, mark.X - 5.0F, mark.Y - 5.0F, 5.0F, 5.0F);  
                    addOrRemoveRouteMark(true, 1, mark);
                }
            }
            if (gReedyRoute != null)
            {
                List<Route.pos_t> gReedy = gReedyRoute.getRoute();
                for (int i = 0; i < gReedy.Count; i++)
                {
                    //float mark_x = (aStar.ElementAt(i).pos.X / 100.0F) * panel1.Width;
                    //float mark_y = (aStar.ElementAt(i).pos.Y / 100.0F) * panel1.Height;
                    // window pos = (route pos / 100) * window size
                    //g.DrawEllipse(penRoute, mark_x - 5.0F, mark_y - 5.0F, 5.0F, 5.0F);
                    PointF mark = new PointF();
                    mark = calcRoutePoint(gReedy.ElementAt(i));
                    // window pos = (route pos / 100) * window size
                    //g.DrawEllipse(pen, mark.X - 5.0F, mark.Y - 5.0F, 5.0F, 5.0F);  
                    addOrRemoveRouteMark(true, 2, mark);
                }
            }
        }

        private void Form1_ResizeEnd(object sender, EventArgs e) => Refresh();

        

        private void generateBtn_Click(object sender, EventArgs e)
        {
            timer1.Stop();
            _maze = new Maze(new Size((int) numericSize.Value, (int)numericSize.Value), (int)numericAdditionalDoors.Value);
            if (aStarRoute != null)
            {
                aStarRoute.clearRoute();
                //aStarRouteCount = 0;
                aStarCpy.Clear();
            }
            if (gReedyRoute != null)
            {
                gReedyRoute.clearRoute();
                //aStarRouteCount = 0;
                gReedyCpy.Clear();
            }
            aStarDone = false;
            gReedyDone = false;
            Refresh();
        }

        private void checkedListBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void startBtn_Click(object sender, EventArgs e)
        {
            if (checkedListBox1.GetItemChecked(0) == true)
            {
                if (aStarRoute == null)
                {
                    aStarRoute = new AStarRoute();
                }
                int tarRoom = (_maze.Size.Width * _maze.Size.Height) - 1;
                Size rooms = _maze.Size;
                aStarRoute.routeStart(0, tarRoom, rooms, _maze.Doors.ToList<Door>());
                //aStarRouteCount = 0;
                aStarCpy.Clear();
            }

            if (checkedListBox1.GetItemChecked(1) == true)
            {
                if (gReedyRoute == null)
                {
                    gReedyRoute = new GreedyRoute();
                }
                int tarRoom = (_maze.Size.Width * _maze.Size.Height) - 1;
                Size rooms = _maze.Size;
                gReedyRoute.routeStart(0, tarRoom, rooms, _maze.Doors.ToList<Door>());
                gReedyCpy.Clear();
            }

            if ((checkedListBox1.GetItemChecked(0) == true) || (checkedListBox1.GetItemChecked(1) == true))
            {
                timer1.Start();
            }
        }

        private void stopBtn_Click(object sender, EventArgs e)
        {
            timer1.Stop();
        }

        private PointF calcRoutePoint( Route.pos_t routePos)
        {
            PointF mark = new PointF();
            mark.X = (routePos.pos.X / 100.0F) * panel1.Width;
            mark.Y = (routePos.pos.Y / 100.0F) * panel1.Height;
            return mark;
        }

        private void addOrRemoveRouteMark( bool add, int router, PointF mark)
        {
            Graphics g = panel1.CreateGraphics();

            if (add == false)
            {
                //float markInvSize = (panel1.Width / (float)numericSize.Value) * 1.0F;
                //float markInvOffset = markInvSize + (markInvSize * 0.1F);
                PointF markInvSize = new PointF();
                markInvSize.X = (((float)panel1.Width / (float)numericSize.Value) * 0.5F) + 0.5F;
                markInvSize.Y = (((float)panel1.Height / (float)numericSize.Value) * 0.5F) + 0.5F;
                PointF markInvOffset = new PointF();
                //markInvOffset.X = markInvSize.X + (markInvSize.X * 0.2F);
                //markInvOffset.Y = markInvSize.Y + (markInvSize.Y * 0.2F);
                
                if (router == 1)
                {
                    markInvOffset.X = markInvSize.X - 2.0F;
                    markInvOffset.Y = markInvSize.Y - 2.0F;
                    Rectangle rectInvalidate = new Rectangle( (int)(mark.X - markInvOffset.X), 
                                                              (int)(mark.Y - markInvOffset.Y), 
                                                              (int)(markInvSize.X), 
                                                              (int)(markInvSize.Y));
                    panel1.Invalidate(rectInvalidate);
                }
                else
                {
                    markInvOffset.X = 0.0F;
                    markInvOffset.Y = 0.0F;
                    Rectangle rectInvalidate = new Rectangle((int)(mark.X + markInvOffset.X),
                                                             (int)(mark.Y + markInvOffset.Y),
                                                             (int)(markInvSize.X),
                                                             (int)(markInvSize.Y));
                    panel1.Invalidate(rectInvalidate);
                }
            }
            else
            {
                PointF markEllSize = new PointF();
                markEllSize.X = (((float)panel1.Width / (float)numericSize.Value) * 0.25F);
                markEllSize.Y = (((float)panel1.Height / (float)numericSize.Value) * 0.25F);
                PointF markEllOffset = new PointF();
                if (router == 1)
                {
                    markEllOffset.X = markEllSize.X + (markEllSize.X * 0.3F);
                    markEllOffset.Y = markEllSize.Y + (markEllSize.Y * 0.3F);
                    var pen = new Pen(Color.Green, 1);                    
                    g.DrawEllipse(pen, mark.X - markEllOffset.X, mark.Y - markEllOffset.Y, markEllSize.X, markEllSize.Y);
                }
                else
                {
                    markEllOffset.X = markEllSize.X - (markEllSize.X * 0.3F);
                    markEllOffset.Y = markEllSize.Y - (markEllSize.Y * 0.3F);
                    var pen = new Pen(Color.Blue, 1);
                    g.DrawEllipse(pen, mark.X + markEllOffset.X, mark.Y + markEllOffset.Y, markEllSize.X, markEllSize.Y);
                }
            }
            
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            //Graphics g = panel1.CreateGraphics();
            //var pen = new Pen(Color.Green, 1);

            if( aStarRoute != null )
            {
                if (aStarRoute.getFinished() == false)
                {
                    aStarRoute.routeProcess();
                    List<Route.pos_t> aStar = aStarRoute.getRoute();
                    PointF mark = new PointF();
                    if (aStar.Count <= aStarCpy.Count)
                    {
                        mark = calcRoutePoint(aStarCpy.Last());
                        //Rectangle rectInvalidate = new Rectangle((int)(mark.X - 10.0F), (int)(mark.Y - 10.0F), 20, 20);
                        //panel1.Invalidate(rectInvalidate);
                        addOrRemoveRouteMark(false, 1, mark);
                        if (aStar.Count == aStarCpy.Count)
                        {
                            mark = calcRoutePoint(aStar.Last());
                            // window pos = (route pos / 100) * window size
                            //g.DrawEllipse(pen, mark.X - 5.0F, mark.Y - 5.0F, 5.0F, 5.0F);
                            addOrRemoveRouteMark(true, 1, mark);
                        }
                    }
                    else
                    {
                        mark = calcRoutePoint(aStar.Last());
                        // window pos = (route pos / 100) * window size
                        //g.DrawEllipse(pen, mark.X - 5.0F, mark.Y - 5.0F, 5.0F, 5.0F);  
                        addOrRemoveRouteMark(true, 1, mark);
                    }
                    aStarCpy = aStar.ToList();
                    //aStarRouteCount = aStar.Count;
                }
                else
                {
                    aStarDone = true;
                }
            }

            if (gReedyRoute != null)
            {
                if (gReedyRoute.getRoutesFound() == 0 )
                {
                    gReedyRoute.routeProcess();
                    List<Route.pos_t> gReedy = gReedyRoute.getRoute();
                    PointF mark = new PointF();
                    if (gReedy.Count <= gReedyCpy.Count)
                    {
                        mark = calcRoutePoint(gReedyCpy.Last());
                        addOrRemoveRouteMark(false, 2, mark);
                        if (gReedy.Count == gReedyCpy.Count)
                        {
                            mark = calcRoutePoint(gReedy.Last());
                            addOrRemoveRouteMark(true, 2, mark);
                        }
                    }
                    else
                    {
                        mark = calcRoutePoint(gReedy.Last());
                        addOrRemoveRouteMark(true, 2, mark);
                    }
                    gReedyCpy = gReedy.ToList();
                }
                else
                {
                    gReedyDone = true;
                }
            }

            if ((gReedyDone == true) && (aStarDone == true))
            {
                Refresh();
                timer1.Stop();
            }

        }

        private void numericSize_ValueChanged(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void numericAdditionalDoors_ValueChanged(object sender, EventArgs e)
        {

        }
    }
}