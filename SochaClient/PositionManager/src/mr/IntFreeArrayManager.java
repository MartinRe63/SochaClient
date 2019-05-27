package mr;

public class IntFreeArrayManager {
	int [] toManageOn;
	int firstFree;
	int lastFree;
	int freeMask = Integer.MIN_VALUE;
	int initValue = 0;
	int blockSize = 1;
	int reservedBlockCnt = 0;  
	
	private void Init( int[] arrayToManageOn ) {
		toManageOn = arrayToManageOn;
		
		int i;
		for ( i = 0; i < arrayToManageOn.length; i+=blockSize) {
			arrayToManageOn[i] = (i+blockSize) | freeMask;
			// i overrun should show exception
		}
		toManageOn[i-blockSize] = Integer.MAX_VALUE ^ freeMask;
		firstFree = 0;
	}
	
	public IntFreeArrayManager( int[] arrayToManageOn ) 
	{
		Init (arrayToManageOn );
	}
	
	public IntFreeArrayManager( int[] arrayToManageOn, int BlockSize )  
	{
		blockSize = BlockSize;
		Init (arrayToManageOn );
	}
	
	public int GetFree()
	{
		return firstFree;
	}
	
	private int Reserve( int Index )  throws Exception
	{
		if ( ! IsFree( Index ) ) throw new Exception("Element at Index:" + Index + " is not free.");
		if ( Index != firstFree ) throw new Exception("You can only reserve the next free position. - Use ReserveNextFree");
		toManageOn[Index] ^= freeMask; // switch of signal free bit
		firstFree = (int)(toManageOn[Index]);
		toManageOn[Index] = initValue; 
		reservedBlockCnt++;
		return Index;
	}
	
	public void Dispose( int Index ) throws Exception
	{
		if ( IsFree( Index ) ) throw new Exception("Element at Index:" + Index + " is already disposed.");
		if ( Index % blockSize != 0 ) throw new Exception("Element at Index:" + Index + " is not a block begin.");
		toManageOn[Index] = freeMask ^ firstFree;
		firstFree = Index;
		reservedBlockCnt--;
	}
	
	public boolean IsFree( int Index ) throws Exception	
	{
		if ( Index % blockSize != 0 ) throw new Exception("Element at Index:" + Index + " is not a block begin.");
		return ( toManageOn[Index] & freeMask ) != 0;
	}
	
	public int ReserveNextFree() throws Exception
	{
		if ( firstFree == Integer.MAX_VALUE ) throw new Exception ( "No free Element available." );
		return (Reserve( firstFree ));
	}
	
	public int FreeBlocks()
	{
		return ( toManageOn.length / blockSize - reservedBlockCnt ) ;
	}
	
	public int ReservedBlocks()
	{
		return reservedBlockCnt;
	}

}
