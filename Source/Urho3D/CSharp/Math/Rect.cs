using System;
using System.Runtime.InteropServices;

namespace Urho3D
{
	[StructLayout(LayoutKind.Sequential)]
	public struct Rect
	{
        /// Minimum vector.
        public Vector2 Min;
        /// Maximum vector.
        public Vector2 Max;

		public Rect(float minX, float minY, float maxX, float maxY)
		{
			Min = new Vector2(minX, minY);
			Max = new Vector2(maxX, maxY);
		}

	    public Rect(Vector2 min, Vector2 max)
	    {
	        Min = min;
	        Max = max;
	    }

	    /// Merge a point.
	    public void Merge(Vector2 point)
	    {
	        if (point.X < Min.X)
	            Min.X = point.X;
	        if (point.X > Max.X)
	            Max.X = point.X;
	        if (point.Y < Min.Y)
	            Min.Y = point.Y;
	        if (point.Y > Max.Y)
	            Max.Y = point.Y;
	    }

	    /// Merge a rect.
	    public void Merge(Rect rect)
	    {
	        if (rect.Min.X < Min.X)
	            Min.X = rect.Min.X;
	        if (rect.Min.Y < Min.Y)
	            Min.Y = rect.Min.Y;
	        if (rect.Max.X > Max.X)
	            Max.X = rect.Max.X;
	        if (rect.Max.Y > Max.Y)
	            Max.Y = rect.Max.Y;
	    }

		public static readonly Rect Full = new Rect(-1.0f, -1.0f, 1.0f, 1.0f);
		public static readonly Rect Positive = new Rect(0.0f, 0.0f, 1.0f, 1.0f);
		public static readonly Rect Zero = new Rect(0.0f, 0.0f, 0.0f, 0.0f);
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct IntRect
	{
        /// Left coordinate.
        public int Left;
        /// Top coordinate.
        public int Top;
        /// Right coordinate.
        public int Right;
        /// Bottom coordinate.
        public int Bottom;

		public IntRect(int left, int top, int right, int bottom)
		{
			Left = left;
			Top = top;
			Right = right;
			Bottom = bottom;
		}

		public static readonly IntRect Zero = new IntRect(0, 0, 0, 0);

	    public IntRect(IntVector2 min, IntVector2 max)
	    {
	        Left = min.X;
	        Top = min.Y;
	        Right = max.X;
	        Bottom = max.Y;
	    }

	    public int Width => Right - Left;
	    public int Height => Bottom - Top;
	    public IntVector2 Size => new IntVector2(Width, Height);

	    public static bool operator !=(IntRect lhs, IntRect rhs)
	    {
	        return lhs.Top != rhs.Top || lhs.Right != rhs.Right || lhs.Bottom != rhs.Bottom || lhs.Left != rhs.Left;
	    }

	    public static bool operator ==(IntRect lhs, IntRect rhs)
	    {
	        return !(lhs != rhs);
	    }
	}
}
