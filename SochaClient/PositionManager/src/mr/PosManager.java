package mr;

import java.math.BigInteger;

public class PosManager {
	public static class GameEndException extends Exception
	{
		public double res;
		GameEndException( double Result ) {
			super("Game End to be catched.");
			res = Result;
		}
	}
	
	public static void arrayCopy(long[][] aSource, long[][] aDestination) {
	    for (int i = 0; i < aSource.length; i++) {
	        System.arraycopy(aSource[i], 0, aDestination[i], 0, aSource[i].length);
	    }
	}
	public static int getMove(int color, long[][] aSource, long[][] aDestination)
	{
		long[][] mask = new long[3][2];
	    for (int i = 0; i < aSource.length; i++) 
		    for (int j = 0; j < aSource[i].length; j++)
		    	mask[i][j] = aSource[i][j] ^ aDestination[i][j];
	    int from = Long.numberOfTrailingZeros( mask[color][0] & aSource[color][0] );
	    	from += Long.numberOfTrailingZeros( mask[color][1] & aSource[color][1] );
	    int to = Long.numberOfTrailingZeros( mask[color][0] & aDestination[color][0] );
	    	to += Long.numberOfTrailingZeros( mask[color][1] & aDestination[color][1] );
	    return PosManager.PackMove( from, to );
	}
		
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
	public static long [][] FromString( String Init )
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
				  ". . . . . . . . . . \r\n" +
				  ". . . . . . . . . . \r\n" +
				  ". . . . . . . . . . \r\n" +
				  ". . . . . . . . . . \r\n" +
    			  ". . . . . . . . . . \r\n" +
				  ". . . . . . . . . . \r\n" +
				  ". . . . . . . . . . \r\n" +
				  ". . . . . . . . . . \r\n" +
				  ". . . . . . . . . . \r\n" +
				  ". . . . . . . . . . \r\n" ); 
				;
		char[] res1 = res.toCharArray();
		for( int y = 0; y < 10; y++)
		{
			for ( int x = 0; x < 10; x++)
			{
				if ( IsBit(Pos[0], y*10+x ) )
					res1[(9-y)*22+x*2] = '0';
				else if ( IsBit(Pos[1], y*10+x )  )
					res1[(9-y)*22+x*2] = '1';
				else if ( IsBit( Pos[2], y*10+x ) )
					res1[(9-y)*22+x*2] = 'C';
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
		int val = (int)'a';
		return new String(String.valueOf((char)(val+p%10) + String.valueOf(p/10)));
	}
	public static String packMoveToString( int move )
	{
		int[] moves =  getYX( move );
		String res = new String(PosToString(moves[0])+"->"+PosToString(moves[1])); 
		return res;		
	}
	public static int PackMove( int moveFrom, int moveTo )
	{
		int[]move = new int[] {moveFrom, moveTo};
		return PackMove(move);
	}
	

	public static int moveValue (int move )
	{
		int mask = 15;
		int fromX = ( move & mask ); mask <<= 4;
		int fromY = ( move & mask ) >> 4; mask <<= 4;
		int toX = ( move & mask ) >> 8; mask <<= 4;
		int toY = ( move & mask ) >> 12;
		
		int toVal = ( toX >= 5 ? 9-toX : toX ) * (toY >=5 ? 9-toY : toY );
		int fromVal = ( fromX >= 5 ? 9-fromX : fromX ) * ( fromY >=5 ? 9-fromY : fromY );
		return ( toVal - fromVal );  
	}
	public static long blockValue ( long[] blockData )
	{
		long ret = Long.bitCount(blockData[0]) + Long.bitCount(blockData[1]);
		int val = 4;
		while ( ( blockData[0] & MaskManager.fishValueMasks[val][0] ) == 0 && (blockData[1] & MaskManager.fishValueMasks[val][1]) == 0)
			val--;
		long multiplier = val > 1 ? 8 : val;
		for ( int i = 3; i <= val; i++ ) multiplier <<= 3;
		ret *= multiplier;
		return ret;
	}
	public static int numberOfTrailingZeros( long low, long high )
	{
		int ret = Long.numberOfTrailingZeros(low);
		if ( ret == 64 )
			ret += Long.numberOfTrailingZeros(high);
		return ret;
	}
	public static int getFirstRightBitPos ( long low, long high )
	{
		return numberOfTrailingZeros( low, high ); 
	}
	public static int getNextRightBitPos( long low, long high, int currentPos)
	{
		assert ( getFirstRightBitPos(low, high) >= currentPos ) : "bit found before " + currentPos;
		if ( currentPos >= 64 )
		{
			high &= ~(1L << (currentPos - 64));  // switch of current pos in copied data
		}
		else 
		{
			low &=  ~(1L << currentPos);
		}
		return ( getFirstRightBitPos( low, high ));
	}
	public static int getNextRightBitPosIgnorePrevious( long low, long high, int currentPos)
	{
		int p = getFirstRightBitPos(low, high);
		// switch of current pos in copied data
		while ( p < currentPos )
		{
			if ( p >= 64 ) 		{
				high &= ~(1L << p - 64);  
			} else 	{
				low &=  ~(1L << p);
			}
			p = getFirstRightBitPos(low, high);
		}
		if ( p > currentPos ) 
			return p;
		else
			return getNextRightBitPos( low, high, p );
	}
	/**
	 * !!! careful ... This will change the current pos value
	 * @param pos
	 * @param currentPos
	 * @return
	 */
	public static int getNextRightBitPos( long[] pos, int currentPos)
	{
		assert ( getFirstRightBitPos(pos[0], pos[1]) == currentPos ) : currentPos + "bit found before " + currentPos;
		if ( currentPos >= 64 )
		{
			pos[1] &= ~(1L << currentPos - 64);  // switch of current pos in the current position
		}
		else 
		{
			pos[0] &= ~(1L << currentPos);
		}
		return ( getFirstRightBitPos( pos[0], pos[1] ));
	}

/*	
	public static int getNextFishPos( long low, long high, int currentPos )
	{
		int ret = currentPos+1;
		long[] Data = new long[] { low, high };
		long mask = 1L << ret % 64;
		long toCheck = Data[ret / 64];
		while ( ( toCheck & mask ) == 0 && ( ret % 8 ) != 0) 
		{
			mask <<= 1;
			ret ++;
		}
		if ( ( toCheck & mask ) != 0 )
			return ret;
		if ( ret == 64 )
		{
			toCheck = Data[ 1 ];
		}
		mask = 255L << ret ; // (8 bit mask)
		String s = "";
		while ( ( toCheck & mask ) == 0 )
		{
			mask <<= 8;
			ret += 8;
			if ( ret == 64 )
			{
				toCheck = Data[ret / 64];
				mask = 255L;
			}
			// s = Long.toString(mask, 2) + " " + Long.toString(toCheck, 2);
			assert ret < 128 : "Stop endless loop - an error occured - something went wrong."; 
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
	
*/	
	/**
	 * Extend a block of fishes for a number of new fishes
	 * @param posData      - posData fishes not in the block and not new for the current block
	 * @param blockList    - fishes already considered for the block / 0 = lowFishes - 1 = highFishes - 2 = maxValue 
	 * @param newFishes    - new fishes for the current block
	 * @return
	 */
	public static void extendBlock( long[]posData, long[]block, long newFishesLow, long newFishesHigh, int firstFishPos )
	{
		assert (newFishesLow != 0) || (newFishesHigh != 0) : "Software Issue - no new fishes found.";  
		int bitCnt = Long.bitCount(newFishesLow) + Long.bitCount(newFishesHigh);
		int bitPos = firstFishPos;
		for( int k=0; k < bitCnt; k++)
		{
			PosManager.SetBit(block, bitPos);
			long foundNewFishesLow = MaskManager.neighborMasks[bitPos][0] & ( posData[0] ^ block[0] );
			long foundNewFishesHigh = MaskManager.neighborMasks[bitPos][1] & ( posData[1] ^ block[1] );
			if ( Long.bitCount(foundNewFishesLow) > 0 || Long.bitCount(foundNewFishesHigh) > 0)
			{
				int firstPos = getFirstRightBitPos( foundNewFishesLow, foundNewFishesHigh );
				extendBlock( posData, block, foundNewFishesLow, foundNewFishesHigh, firstPos );
			}
			if ( k+1 < bitCnt )
				bitPos = getNextRightBitPosIgnorePrevious(newFishesLow, newFishesHigh, bitPos);
		}
	}
	
	public static int getBlockAndCnt( long[] posData, long[][] blockList) 
	{
		assert blockList.length >= 16 : "Software Issue. A blocklist can maximum contain 16 blocks.";
		assert blockList[15].length >= 2 : "Software Issue. A blocklist hold the low and high long of the fishes.";
		int cnt = 0;
		int bitToFind = PosManager.BitCnt( posData );
		long restFishesLow = posData[0]; 
		long restFishesHigh = posData[1]; 
		while ( bitToFind > 0 )
		{
			for ( int k = 0; k < 2; k++ ) blockList[cnt][k] = 0; 
			int bitId = getFirstRightBitPos( restFishesLow, restFishesHigh );
			PosManager.SetBit(blockList[cnt], bitId); // use empty blocklist Storage to set the first fish of a block
			extendBlock( posData, blockList[cnt], blockList[cnt][0], blockList[cnt][1], bitId );
			int bitFound = PosManager.BitCnt(blockList[cnt]);
			assert bitFound > 0 : "Software Issue. A block must contain in minimum 1 fish, otherwise this is an endless loop.";
			bitToFind -= bitFound; 
			// switch off fishes found
			restFishesLow ^= blockList[cnt][0];
			restFishesHigh ^= blockList[cnt][1];
			cnt++;
		}
		return cnt;
	}
	/**
	 * check if the end is reached (available moves checked afterwards) and if yes the result
	 * @param blockListAll
	 * @param pos
	 * @return
	 * -1 : End not reached
	 * 0 : End reached -> Winner is blue
	 * 1 : End reached -> Winner is red
	 * 0.5 : End reached -> no Winner
	 */
	public static double Analysis( int moveCnt, long[][][] blockListAll, int[] blockCount, long[][] pos, int MaxDepth )
	{
		boolean roundEnd = moveCnt % 2 == 0 && moveCnt > 0;
		boolean gameEnd = moveCnt >= MaxDepth || ( roundEnd && ( blockCount[0] == 1 || blockCount[1] == 1 ) );

		if ( gameEnd || roundEnd )
		{
			if ( blockCount[0] == 1 && blockCount[1] == 1 )
				return 0.5;
			else if ( blockCount[0] == 1)
				return 1;
			else if ( blockCount[1] == 1)
				return 0;
			
		}
		if ( gameEnd )
		{
			int maxRed = 0; int maxBlue = 0; int cnt;
			for( int k = 0; k <= blockCount[0]; k++)
				if ( ( cnt = ( Long.bitCount(blockListAll[0][k][0]) + Long.bitCount(blockListAll[0][k][1]) ) ) > maxRed )
				{
					maxRed = cnt;
				}
			for( int k = 0; k <= blockCount[1]; k++)
				if ( ( cnt = ( Long.bitCount(blockListAll[1][k][0]) + Long.bitCount(blockListAll[1][k][1]) ) ) > maxBlue )
				{
					maxBlue = cnt;
				}
			if ( maxRed == maxBlue )
				return 0.5;
			else if ( maxRed > maxBlue )
				return 1;
			else 
				return 0;
		}
		return -1;
	}

	public static double factor = 0.3 / (512*16); 
	
	public static double GetValue( long[][]pos, int color, long[][][] blockList, int[] blockCnt, int depth, int firstMoveDepth ) throws GameEndException
	{
		// calculate the value of this position
		// here to count number of blocks and calculate the block value
		// check if this is the secondMoveColor to check if moveColor will win = 1 or loss = 0
		long valColor = PosManager.getPosValue( pos, color, blockList, blockCnt );
		long valOppositeColor = PosManager.getPosValue( pos, (color+1) % 2, blockList, blockCnt );
		double ret;
		// foundGameEnd = false;
		if ( ( ret = PosManager.Analysis(depth, blockList, blockCnt, pos, 60-firstMoveDepth) ) < 0 )
		{
			ret  = ( valColor - valOppositeColor ) * factor + 0.5;
		}
		else 
		{
			// foundGameEnd = true;
			if ( color == 1 )
			{
				ret = 1 - ret;
			}
			throw new GameEndException( ret );		
		}
		return ret;
	}
	public static String AnalysisToString( double val )
	{
		String ret = "The winner is ";
		if ( val == 1.0 )
			return ret + "red"; 
		else if ( val == 0.0)
			return ret + "blue";
		else
			return "The game ends unentschieden.";
	}

	public static long getPosValue(long[][] pos, int color, long[][][]blockList, int[] blockCnt)
	{
		// System.out.println(PosManager.ToString( pos1 ));
		blockCnt[color] = PosManager.getBlockAndCnt(pos[color], blockList[color]);
		long posValue = 0;
		for ( int i = 0; i < blockCnt[color]; i++)
		{
			posValue += PosManager.blockValue(blockList[color][i]);
		}
		return posValue;
	}
}

