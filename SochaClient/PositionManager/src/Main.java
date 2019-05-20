import java.math.BigInteger;
import java.util.Arrays;
import java.util.Random;

public class Main {
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
		//PosManager.DebugOut( Pos );
		int[] move = new int[1]; 
		// move[0] = PosManager.PackMove(new int[]{ 8, 29 });
		// String s = BigInteger.valueOf(move[0]).toString(2);
		// NodeManager.movePosition(0, move, Pos, 1);
		long[][] blockList = new long[16][2]; 
		
		int blockCnt = PosManager.getBlockAndCnt(Pos[0], blockList);
		for ( int i=0; i < blockCnt; i++)
		{
			long [][] Pos1 = Pos.clone();
			Pos1[0][0] &= blockList[i][0];
			Pos1[0][1] &= blockList[i][1];
			System.out.println(PosManager.ToString( Pos1 ));
		}
	}
}
