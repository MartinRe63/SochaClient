import java.math.BigInteger;
import java.util.Random;

public class Main {
	public static void main(String[] args) 
	{
		long test = 0B0111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111L;
		// MaskManager.initMasks(); 
		
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
		move[0] = PosManager.PackMove(new int[]{ 8, 29 });
		String s = BigInteger.valueOf(move[0]).toString(2);
		NodeManager.movePosition(0, move, Pos, 1);
		System.out.println(PosManager.ToString( Pos ));
	}
}
