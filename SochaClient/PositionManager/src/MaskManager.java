
public class MaskManager {
	public static long directionMasks[][][] = new long[4][100][2];
	//
	// Die Move Masken haben eine doppelte Nutzung
	// 1. Zur Prüfung ob in die jeweilige Raumrichtung gegnerische Steine oder Kraken sind (Use MoveLenght-1)
	// 2. Zur Prüfung ob am Zielpunkt ein gegnerischer Stein ist, der entfernt werden muss (Einfach ausschalten - Ob dort ein Stein ist, ist unwichtig) 
	public static long moveMasks[][][][] = new long [8][10][100][2]; // DIMENSIONS - Direction, MoveLength, Positionen, High/Low Bits

	public static long neighborMasks[][] = new long [100][2];
	
	// A fish at 0,x or x,0 has a value 0
	// A fish at 1,x or x,1 has a value 1
	public static long fishValueMasks[][] = new long[5][2];  // Dimension - value, bitmask
	
	public static void initMasks() {
		int xMove = 0;
		int yMove = 0;
		for ( int dir = 0; dir < 4; dir++ )
		{
			if ( dir == 0 ) { xMove = 1; yMove = 0; }
			if ( dir == 1 ) { xMove = 1; yMove = 1; }
			if ( dir == 2 ) { xMove = 0; yMove = 1; }
			if ( dir == 3 ) { xMove = -1; yMove = 1; }
			for ( int y = 0; y < 10; y++)
			{
				for ( int x = 0; x < 10; x++)
				{
					long lowMask = 0;
					long highMask = 0;
					for ( int corr = 1; corr > -2; corr-= 2 )
					{
						int nextX = x;
						int nextY = y;
						int bitCnt = 0;
						int moveCnt = 0;
						long lowMoveMask = 0;
						long highMoveMask = 0;
						while ( nextX < 10 && nextX >= 0 && nextY < 10 && nextY >= 0 ) 
						{
							bitCnt = nextX + nextY * 10;
							lowMask |= (bitCnt < 64 ? (1L << bitCnt ) : 0);
							highMask |= (bitCnt >= 64 ? ( 1L << (bitCnt - 64)) : 0);
							
							if ( moveCnt > 0 ) 
							{
								lowMoveMask |= (bitCnt < 64 ? (1L << bitCnt ) : 0);
								highMoveMask |= (bitCnt >= 64 ? ( 1L << (bitCnt - 64)) : 0);
								moveMasks[corr == 1 ? dir : dir + 4][moveCnt][y*10+x][0] |= lowMoveMask;
								moveMasks[corr == 1 ? dir : dir + 4][moveCnt][y*10+x][1] |= highMoveMask;
								// System.out.println("dir=" + (corr == 1 ? dir : dir + 4) + " moveCnt=" + moveCnt + " coord=" + (y*10+x) + ">" + Long.toBinaryString(highMoveMask) + " " + Long.toBinaryString(lowMoveMask));
								if (moveCnt == 1) {
									neighborMasks[y*10+x][0] |= lowMoveMask;
									neighborMasks[y*10+x][1] |= highMoveMask;
									System.out.println("dir=" + (corr == 1 ? dir : dir + 4) + " coord=" + (y*10+x) + ">" + Long.toBinaryString(neighborMasks[y*10+x][1]) + " " + Long.toBinaryString(neighborMasks[y*10+x][0]));
								}
							}
							
							nextX += xMove * corr;
							nextY += yMove * corr;
							moveCnt++;
						}
					}
					directionMasks[dir][y*10+x][0] = lowMask;
					directionMasks[dir][y*10+x][1] = highMask;
					// System.out.println("dir=" + dir + " coord=" + (y*10+x) + ">" + Long.toBinaryString(highMask) + " " + Long.toBinaryString(lowMask));
				}
			}
		}
		for ( int y= 0; y < 10; y++)
		{
			for ( int x = 0; x < 10; x++)
			{
				int val = Integer.min(x >= 5 ? 9-x : x, y >= 5 ? 9-y : y);
				PosManager.SetBit(fishValueMasks[val], y*10+x);
			}
		}
	}
    public static void initGame() {
    	String field = 
    	  ".11111111." +
    	  "0........0" +
    	  "0........0" +
    	  "0........0" +
    	  "0...X....0" +
    	  "0....X...0" +
    	  "0........0" +
    	  "0........0" +
    	  "0........0" +
    	  ".11111111.";
    }
}

