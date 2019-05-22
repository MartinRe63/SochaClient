import java.math.BigInteger;
import java.util.Arrays;
import java.util.Random;

public class Main {
	public static void arrayCopy(long[][] aSource, long[][] aDestination) {
	    for (int i = 0; i < aSource.length; i++) {
	        System.arraycopy(aSource[i], 0, aDestination[i], 0, aSource[i].length);
	    }
	}
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
			arrayCopy(Pos,Pos1);
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
	
	public static void playSimpleGame(long[][] Pos) 
	{
		int[] moves = new int[16*8];
		
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
		testMoveCalculation(Pos);
			
	}
}
