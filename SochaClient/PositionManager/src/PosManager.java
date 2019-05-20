import java.math.BigInteger;

public class PosManager {
	static void SetBit( long[] pos, int bitId )
	{
		if ( bitId < 64 )
			pos[0] |= ( 1L << bitId );
		else
			pos[1] |= ( 1L << bitId-64 ); 
	}
	static int BitCnt( long[] pos )
	{
		return Long.bitCount(pos[0]) + Long.bitCount(pos[1]);  
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
	/**
	* A bit move (yto - xto - yfrom - xfrom is converted to an array of integer[From x+10*y .. To x+10*y]. 
	*/
	public static int[] getYX( int move )
	{
		int[] res = new int[2];
		int mask = 15;
		int bitMove = move;
		res[0] = bitMove & mask; mask <<= 4;
		res[0] += ( ( bitMove & mask ) >> 4 ) * 10; mask <<= 4;
		res[1] =  ( bitMove & mask ) >> 8; mask <<= 4;
		res[1] += ( ( bitMove & mask ) >> 12 ) * 10; 
		return res;
	}
	public static String PosToString( int p )
	{
		return new String(p%10 + "," + p/10);
	}
	public static String packMoveToString( int move )
	{
		int[] moves =  getYX( move );
		String res = new String("from:"+PosToString(moves[0])+","+PosToString(moves[1])); 
		return res;		
	}
	public static int PackMove( int moveFrom, int moveTo )
	{
		int[]move = new int[] {moveFrom, moveTo};
		return PackMove(move);
	}
	
	
	public static long blockValue ( long[] blockData )
	{
		long ret = Long.bitCount(blockData[0]) + Long.bitCount(blockData[1]);
		int val = 4;
		while ( ( blockData[0] & MaskManager.fishValueMasks[val][0] ) == 0 && (blockData[1] & MaskManager.fishValueMasks[val][1]) == 0)
			val--;
		ret *= 8 << val;
		return ret;
	}
	public static int getNextFishPos( long low, long high, int currentPos )
	{
		int ret = currentPos;
		long[] Data = new long[] { low, high };
		long mask = 1L << ret % 64;
		long toCheck = Data[ret / 64];
		while ( ( toCheck & mask ) == 0 && ( ret % 8 ) != 0) 
		{
			mask <<= 1;
			ret ++;
		}
		mask = 255L << ret; // (8 bit mask)
		while ( ( toCheck & mask ) == 0 )
		{
			mask <<= 8;
			ret += 8;
			if ( ret >= 64 )
			{
				toCheck = Data[ret / 64];
				mask = 255L;
			}
		}
		mask = 1L << ret % 64;
		while ( ( toCheck & mask) == 0 ) 
		{
			mask <<= 1;
			ret ++;
		}
		assert ret < 128 : "software issue - we only have 128 bit fields here";
		return ret;
	}
	/**
	 * Extend a block of fishes for a number of new fishes
	 * @param posData      - posData fishes not in the block and not new for the current block
	 * @param blockList    - fishes already considered for the block
	 * @param newFishes    - new fishes for the current block
	 * @return
	 */
	public static void extendBlock( long[]posData, long[]block, long newFishesLow, long newFishesHigh )
	{
		assert (newFishesLow > 0) || (newFishesHigh > 0) : "Software Issue - no new fishes found.";  
		int bitCnt = Long.bitCount(newFishesLow) + Long.bitCount(newFishesHigh);
		int bitPos = 0;
		for( int k=0; k<bitCnt; k++)
		{
			bitPos = getNextFishPos(newFishesLow, newFishesHigh, bitPos);
			PosManager.SetBit(block, bitPos);
			long foundNewFishesLow = MaskManager.neighborMasks[bitPos][0] & ( posData[0] ^ block[0] );
			long foundNewFishesHigh = MaskManager.neighborMasks[bitPos][1] & ( posData[1] ^ block[1] );
			if ( Long.bitCount(foundNewFishesLow) > 0 || Long.bitCount(foundNewFishesHigh) > 0)
				extendBlock( posData, block, foundNewFishesLow, foundNewFishesHigh );
		}
	}
	
	public static int getBlockAndCnt( long[] posData, long[][] blockList) 
	{
		int cnt = 0;
		long[] initPos = new long[] {0,0};
		int bitToFind = PosManager.BitCnt( posData );
		long restFishesLow = posData[0]; 
		long restFishesHigh = posData[1]; 
		while ( bitToFind > 0 )
		{
			int bitId = getNextFishPos( restFishesLow, restFishesHigh, 0);
			PosManager.SetBit(initPos, bitId);
			extendBlock( posData, blockList[cnt], initPos[0], initPos[1]);
			int bitFound = PosManager.BitCnt(blockList[cnt]);
			assert bitFound > 0 : "Software Issue. A block must contain in minimum 1 fish, otherwise this is an endless loop.";
			bitToFind -= bitFound; 
			cnt++;
			initPos[0] = 0;
			initPos[1] = 0;
		}
		return cnt;
	}

}
