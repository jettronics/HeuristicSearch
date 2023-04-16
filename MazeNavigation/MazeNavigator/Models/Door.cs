using System;
using System.Collections.Generic;
using System.Drawing;

namespace Maze_generator.Models
{
    /// <summary>
    ///     A wall between two cells
    /// </summary>
    public class Door : IEquatable<Door>
    {
        public int Cell1 { get; }
        public int Cell2 { get; }

        public Door(int cell1, int cell2)
        {
            Cell1 = cell1;
            Cell2 = cell2;
        }

        

        #region Equality comparison

        public bool Equals(Door other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return Cell1 == other.Cell1 && Cell2 == other.Cell2;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            return obj.GetType() == GetType() && Equals((Edge) obj);
        }

        public override int GetHashCode()
        {
            unchecked
            {
                return (Cell1*397) ^ Cell2;
            }
        }

        public static bool operator ==(Door left, Door right) => Equals(left, right);
        public static bool operator !=(Door left, Door right) => !Equals(left, right);

        #endregion
    }
}