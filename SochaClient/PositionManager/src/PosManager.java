import java.math.BigInteger;

public class PosManager {
	static void SetBit( long[] pos, int bitId )
	{
		if ( bitId < 64 )
			pos[0] |= ( 1L << bitId );
		else
			pos[1] |= ( 1L << bitId-64 ); 
	}
	static void ClearBit( long[] pos, int bitId )
	{
		if ( bitId < 64 )
			pos[0] &= ~( 1L << bitId );
		else
			pos[1] &= ~( 1L << bitId-64 ); 
	}
	static boolean IsBit( long[] pos, int bitId )
	{
		if ( bitId < 64 )
			return ( ( pos[0] & ( 1L << bitId ) ) ) != 0;
		else
			return ( ( pos[1] & ( 1L << bitId ) ) ) != 0;
	}
	
	static int PackMove( int[] move )
	{
		int fromX = move[0] % 10;
		int fromY = move[0] / 10;
		int toX = move[1] % 10;
		int toY = move[1] / 10;
		return fromX | fromY << 4 | toX << 8 | toY << 12;
	}
	static long [][] FromString( String Init )
	{
		long Pos[][] = new long[3][2];
		for( int y = 0; y < 10; y++)
		{
			for ( int x = 0; x < 10; x++)
			{
				String s = Init.substring( (9-y)*10+x, (9-y)*10+x+1); 
				if ( s.compareTo( "0" ) == 0 )
					SetBit( Pos[0], y*10+x );
				else if ( s.compareTo( "1" ) == 0 )
					SetBit( Pos[1], y*10+x );
				else if ( s.compareTo( "C" ) == 0 )
					SetBit( Pos[2], y*10+x );
			}
		}
		return Pos;
	}
	public static String ToString(long[][] Pos)
	{
		String res = new String(
				  "..........\r\n" +
				  "..........\r\n" + 
				  "..........\r\n" + 
				  "..........\r\n" + 
				  "..........\r\n" + 
				  "..........\r\n" + 
				  "..........\r\n" + 
				  "..........\r\n" + 
				  "..........\r\n" + 
				  "..........\r\n" ); 
				;
		char[] res1 = res.toCharArray();
		for( int y = 0; y < 10; y++)
		{
			for ( int x = 0; x < 10; x++)
			{
				if ( IsBit(Pos[0], y*10+x ) )
					res1[(9-y)*12+x] = '0';
				else if ( IsBit(Pos[1], y*10+x )  )
					res1[(9-y)*12+x] = '1';
				else if ( IsBit( Pos[2], y*10+x ) )
					res1[(9-y)*12+x] = 'C';
			}
		}
		res = new String(res1);
		return res;
	}
	
	private static void PosDebugOut(String s, long[] p)
	{
		System.out.println(s + ":" + BigInteger.valueOf(p[1]).toString(2) + " " + BigInteger.valueOf(p[0]).toString(2) );
	}
	public static void DebugOut(long[][] Pos)
	{
		PosDebugOut("red", Pos[0]);
		PosDebugOut("blue", Pos[1]);
		PosDebugOut("crake", Pos[2]);
	}
	
}
