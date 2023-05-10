using System;
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
        private float xt, yt; 

        public Form()
        {
            InitializeComponent();

            const int initialSize = 15;
            _maze = new Maze(new Size(initialSize, initialSize));
            numericSize.Value = initialSize;

            xt = 10; yt = 20; 

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
        }

        private void Form1_ResizeEnd(object sender, EventArgs e) => Refresh();

        

        private void generateBtn_Click(object sender, EventArgs e)
        {
            _maze = new Maze(new Size((int) numericSize.Value, (int)numericSize.Value));
            Refresh();
        }

        private void checkedListBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void startBtn_Click(object sender, EventArgs e)
        {
            if (checkedListBox1.SelectedIndex == 0)
            {
                if (aStarRoute == null)
                {
                    aStarRoute = new AStarRoute();
                    int tarRoom = (_maze.Size.Width * _maze.Size.Height) - 1;
                    Size rooms = _maze.Size;
                    aStarRoute.routeStart(0, tarRoom, rooms, _maze.Doors.ToList<Door>());                    
                }
                timer1.Start();
            }
            
            if (checkedListBox1.SelectedIndex == 1)
            {
                if (gReedyRoute == null)
                {
                    gReedyRoute = new GreedyRoute();
                    int tarRoom = (_maze.Size.Width * _maze.Size.Height) - 1;
                    Size rooms = _maze.Size;
                    gReedyRoute.routeStart(0, tarRoom, rooms, _maze.Doors.ToList<Door>());
                }
                timer1.Start();
            }
        }

        private void stopBtn_Click(object sender, EventArgs e)
        {
            timer1.Stop();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            Graphics g = panel1.CreateGraphics();
            xt += 1;
            yt += 1;
            var pen = new Pen(Color.Green, 1);
            g.DrawLine(pen, xt, yt, xt + 10, yt + 10);

            if( aStarRoute.getFinished() == false )
            {
                aStarRoute.routeProcess();
            }
        }
    }
}