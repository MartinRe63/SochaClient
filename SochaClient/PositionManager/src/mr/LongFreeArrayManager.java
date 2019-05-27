package mr;

//all negative values are blocked for the free array management 
//max size of the array is Integer.Max_Int
public class LongFreeArrayManager {
	long [] toManageOn;
	int firstFree;
	int lastFree;
	long freeMask = Long.MIN_VALUE;
	long initValue = 0;
	int blockSize = 1;
	int reservedBlockCnt = 0;  
	
	public LongFreeArrayManager( long[] arrayToManageOn ) {
		toManageOn = arrayToManageOn;
		int i = 1;
		for ( i = 0; i < arrayToManageOn.length; i++) {
			arrayToManageOn[i] = ((long)i+1) | freeMask;
			// i overrun should show exception
		}
		toManageOn[toManageOn.length-1] = Integer.MAX_VALUE ^ freeMask;
		firstFree = 0;
	}
	public int GetFree()
	{
		return firstFree;
	}
	public int Reserve( int Index )  throws Exception
	{
		if ( ! IsFree(Index ) ) throw new Exception("Element at Index:" + Index + "is not free.");
		if ( Index != firstFree ) throw new Exception("You can only reserve the next free position. - Use ReserveNextFree");
		toManageOn[Index] ^= freeMask; // switch of signal free bit
		firstFree = (int)(toManageOn[Index]);
		toManageOn[Index] = initValue; 
		reservedBlockCnt++;
		return Index;
	}
	public void Dispose( int Index ) throws Exception
	{
		if ( IsFree( Index ) ) throw new Exception("Element at Index: " + Index + " is already disposed.");
		toManageOn[Index] = freeMask ^ (long)firstFree;
		firstFree = Index;
		reservedBlockCnt--;
	}
	public boolean IsFree( int Index )	
	{
		return ( toManageOn[Index] & freeMask ) != 0;
	}
	public int ReserveNextFree() throws Exception
	{
		if ( firstFree == Integer.MAX_VALUE ) throw new Exception ( "No free Element available." );
		return (Reserve( firstFree ));
	}
	public int FreeBlocks()
	{
		return toManageOn.length - reservedBlockCnt;
	}
	public int ReservedBlocks()
	{
		return reservedBlockCnt;
	}
}
