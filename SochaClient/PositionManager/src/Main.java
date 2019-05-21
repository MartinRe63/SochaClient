import java.math.BigInteger;
import java.util.Arrays;
import java.util.Random;

public class Main {
	public static void arrayCopy(long[][] aSource, long[][] aDestination) {
	    for (int i = 0; i < aSource.length; i++) {
	        System.arraycopy(aSource[i], 0, aDestination[i], 0, aSource[i].length);
	    }
	}
	public static void testBlockAndValueCalculation(long[][] Pos)
	{
		long[][] blockList = new long[16][2]; 
		
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
	public static void main(String[] args) 
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
		int[] move = new int[1]; 
		move[0] = PosManager.PackMove(new int[]{ 40, 44 });
		NodeManager.movePosition(0, move, Pos, 0);

		move[0] = PosManager.PackMove(new int[]{ 20, 43 });
		NodeManager.movePosition(0, move, Pos, 0);

		move[0] = PosManager.PackMove(new int[]{ 10, 42 });
		NodeManager.movePosition(0, move, Pos, 0);

		move[0] = PosManager.PackMove(new int[]{ 30, 41 });
		NodeManager.movePosition(0, move, Pos, 0);

		System.out.println(PosManager.ToString( Pos ));
		testBlockAndValueCalculation(Pos);
	}
}
