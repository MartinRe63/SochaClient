package mr;

import java.math.BigInteger;
import java.util.Arrays;
import java.util.Random;

public class Main {
	public static void setTestPosition1(long[][] Pos)
	{
		int[] move = new int[1]; 
		move[0] = PosManager.PackMove(new int[]{ 40, 44 });
		NodeManager.movePosition(0, move, Pos, 0);
	}
	public static void setTestPosition2(long[][] Pos)
	{
		int[] move = new int[1]; 
		move[0] = PosManager.PackMove(new int[]{ 20, 43 });
		NodeManager.movePosition(0, move, Pos, 0);

		move[0] = PosManager.PackMove(new int[]{ 10, 42 });
		NodeManager.movePosition(0, move, Pos, 0);

		move[0] = PosManager.PackMove(new int[]{ 30, 41 });
		NodeManager.movePosition(0, move, Pos, 0);

		move[0] = PosManager.PackMove(new int[]{ 80, 61 });
		NodeManager.movePosition(0, move, Pos, 0);

		move[0] = PosManager.PackMove(new int[]{ 70, 52 });
		NodeManager.movePosition(0, move, Pos, 0);
		
	}
	public static void testBlockAndValueCalculation(long[][] Pos)
	{
		System.out.println(PosManager.ToString( Pos ));
		long[][] blockList = new long[16][2]; 
		setTestPosition1( Pos );
		setTestPosition2( Pos );
		int blockCnt = PosManager.getBlockAndCnt(Pos[0], blockList);
		long [][] Pos1 = new long[3][2];
		for ( int i=0; i < blockCnt; i++)
		{
			PosManager.arrayCopy(Pos,Pos1);
			Pos1[0][0] &= blockList[i][0];
			Pos1[0][1] &= blockList[i][1];
			System.out.println("BlockValue=" + PosManager.blockValue(blockList[i]));
			System.out.println(PosManager.ToString( Pos1 ));
		}
	}
	
	public static void testMoveCalculation(long[][] Pos ) throws Exception
	{
		int[] moves = new int[16*8];
		setTestPosition1(Pos);
		setTestPosition2(Pos);
		System.out.println(PosManager.ToString( Pos ));
		
        ElapsedTimer t = new ElapsedTimer();
		int cnt = NodeManager.getMoveList(Pos, 0, moves);
		System.out.println(t);
		
		for ( int i=0; i<cnt; i++)
		{
			System.out.println("move "+i + ":" + PosManager.packMoveToString(moves[i]));
		}
		System.out.println("");
		
		t = new ElapsedTimer();
		cnt = NodeManager.getMoveList(Pos, 1, moves);
		System.out.println(t);
		
		for ( int i=0; i<cnt; i++)
		{
			System.out.println("move "+i + ":" + PosManager.packMoveToString(moves[i]));
		}
		System.out.println(t);
	}

	public static int simpleGetBestMoveId(long[][] pos, int color, int[] moves, long[][][]blockList, int[] blockCnt) throws Exception
	{
		long [][] pos1 = new long[3][2];
		long maxValue = 0;
		int bestMoveId = 0;
		int cnt = NodeManager.getMoveList(pos, color, moves);
		long posValue = 0;
		for ( int k=0; k < cnt; k++ )
		{
			PosManager.arrayCopy(pos, pos1);
			NodeManager.movePosition(k, moves, pos1, color);

			if ( ( posValue = PosManager.getPosValue( pos1, color, blockList, blockCnt ) ) > maxValue)
			{
				maxValue = posValue; 
				bestMoveId = k;
			}
		}
		// 
		// fill the blocklist with the best block for later analysis
		// approach with note analysis: return the analysis result here
		// 
		PosManager.arrayCopy(pos, pos1);
		NodeManager.movePosition(bestMoveId, moves, pos1, color);
		blockCnt[color] = PosManager.getBlockAndCnt(pos1[color], blockList[color]);
		return bestMoveId;
	}
	public static void playSimpleGame(long[][] pos) throws Exception 
	{
		int[] moves = new int[16*8]; 				// DIM: moveId
		long[][][] blockList = new long[2][16][2]; 	// DIM: Color, BlockId, HL
		int[] blockCnt = new int[2]; 				// DIM: Color
		int moveCnt = 0;
		double result;
		while ( ( result = PosManager.Analysis(moveCnt, blockList, blockCnt, pos, 60-moveCnt) ) < 0  )
		{
			for ( int color = 0; color < 2; color++ )
			{
				int k = simpleGetBestMoveId( pos, color, moves, blockList, blockCnt );
				NodeManager.movePosition(k, moves, pos, color);
				System.out.println(PosManager.packMoveToString(moves[k]));
				System.out.println(PosManager.ToString( pos ));
				moveCnt++;
			}
		}
		System.out.println( PosManager.AnalysisToString(result) );
	}
	
	public static void testGaming(long[][] Pos)throws Exception
	{
        ElapsedTimer t = new ElapsedTimer();
        
		NodeManager redNM = null;
		NodeManager blueNM = null;
		int redMove = 0;
		int blueMove = 0; 
		int moveCnt = 0;
		long ms = 0;
		
		long[][][] blockList = new long[2][16][2]; 	// DIM: Color, BlockId, HL
		int[] blockCnt = new int[2]; 				// DIM: Color
		long expands = 0; 
		try 
		{
			while ( true )
			{
				t.reset();
				if ( redNM == null ) 
					redNM = new NodeManager(10000000, 0, Pos, false, 0);
				else 
					redNM.DisposeTree( redMove, blueMove );
					
				for ( int k = 0; k < 100000; k++)
				{
					redNM.selectAction(true);
					// System.out.println( nm.LastPositionToString() );
					expands++;
				}
				ms += t.elapsed();
						
				moveCnt++;
				NodeManager.movePosition( ( redMove = redNM.BestMove() ) , Pos, 0);
				System.out.println(moveCnt + ". " + PosManager.packMoveToString( redMove ));
				System.out.println(redNM.ValuesToString());
				System.out.println(PosManager.ToString(Pos));
				
				t.reset();
				if ( blueNM == null )
					blueNM = new NodeManager(10000000, 1, Pos, false, 1);
				else
					blueNM.DisposeTree( blueMove, redMove );
				for ( int k = 0; k < 50000; k++)
				{
					blueNM.selectAction(true);
					// System.out.println( nm.LastPositionToString() );
					expands++;
				}
				ms += t.elapsed();
				
				moveCnt++;
				NodeManager.movePosition( ( blueMove = blueNM.BestMove() ), Pos, 1);
				System.out.println(moveCnt + ". " + PosManager.packMoveToString( blueMove ));
				System.out.println(blueNM.ValuesToString());
				System.out.println(PosManager.ToString(Pos));
				
				PosManager.GetValue( Pos, 1, blockList, blockCnt, moveCnt, 0);
			}
		}
		catch ( PosManager.GameEndException e )
		{
			if ( e.res == 1.0 )
				System.out.println( "Blau hat gewonnen. ");
			else
				System.out.println( "Rot hat gewonnen. ");
		}
		System.out.println("Milliseconds per playout = " + ( ms * 1000 ) / expands );
		
	}
	public static void getMoveFrom2Pos(long[][] Pos)
	{
		long[][] Pos1;
		Pos1 = PosManager.FromString( new String(
				  ".11111111."
				+ "0........0"
				+ "..0......0"
				+ "0.....C..0"
				+ "0........0"
				+ "0........0"
				+ "0..C.....0"
				+ "0........0"
				+ "0........0"
				+ ".11111111." )
				 );
		System.out.println( PosManager.packMoveToString( PosManager.getMove(0, Pos, Pos1) ) );
	}

	public static void main(String[] args) throws Exception 
	{
		// long test = 0B0111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111L;
		MaskManager.initMasks(); /// !!! don't forget this !!! 
		
		long[][] Pos;
		Pos = PosManager.FromString( new String(
				  ".11111111."
				+ "0........0"
				+ "0........0"
				+ "0.....C..0"
				+ "0........0"
				+ "0........0"
				+ "0..C.....0"
				+ "0........0"
				+ "0........0"
				+ ".11111111." )
				 );
		// PosManager.DebugOut( Pos );
		// String s = BigInteger.valueOf(move[0]).toString(2);
		// testBlockAndValueCalculation(Pos);
		// testMoveCalculation(Pos);
		// playSimpleGame(Pos);
		testGaming(Pos);
		// getMoveFrom2Pos(Pos);
		
	}
}
