package mr;

import java.util.Iterator;

// The first item in the list keeps the list lenght
// The last item in each block is the pointer to the next block (-1 is the null pointer)
 
/**
 * Manage an Integer List 
 */
public class IntListManager {
	private static final int endMarker = -1;
	int blockSize; 
	int[] arr;
	int freeMask = Integer.MIN_VALUE;
	IntFreeArrayManager ifam; 

	public class OutOfBlockException extends Exception
	{
		private static final long serialVersionUID = 1L;
	    private int lastBlockPos = -1;

		public OutOfBlockException( int LastBlockPos ) 
		{
			super("List Management is working wrong - Missing Block");
			lastBlockPos = LastBlockPos;
		}
	}

	class IntListIterator {
		int listIndex = 0;
		int index = 0;
		IntListIterator( int ListIndex )
		{
			listIndex = ListIndex;  
		}
		public int Lth()
		{
			return GetLength(listIndex);
		}
		private int lastVirtualIndex = -2;
		private int lastArrIndex = -2;
		public int GetItem( int virtualIndex ) throws Exception
		{
			if (virtualIndex >= GetLength(listIndex)) throw new Exception("End of List reached.");

			if ( virtualIndex == lastVirtualIndex + 1 )
			{
				// Check Block Rollover
				lastArrIndex++;
				if ( (lastArrIndex+1) % blockSize == 0 )
				{
					lastArrIndex = arr[lastArrIndex];
				}
			}
			else {
				// From Scratch
				lastArrIndex = GetArrPos(listIndex, virtualIndex);  
			}
			lastVirtualIndex = virtualIndex;
			return arr[ lastArrIndex ];
		}
	}
	IntListIterator getIterator( int ListIndex )
	{
		return new IntListIterator( ListIndex );
	}
	
	IntListManager( int[] Arr, int BlockSize) throws Exception
	{
		arr = Arr; 
		if ( BlockSize < 4 ) throw new Exception("A Blocksize lower 4 is useless.");
		blockSize = BlockSize;
		ifam = new IntFreeArrayManager(Arr, BlockSize);
	}
	public int ReserveList() throws Exception
	{
		// returns the list position
		int listPos = ifam.ReserveNextFree(); 
		arr[listPos] = 0;
		arr[listPos + blockSize - 1] = endMarker;
		return listPos;		 
	}
	
	private int GetArrPos( int ListPos, int Idx ) throws OutOfBlockException, Exception
	{
		// Idx++; // jump over length information, but to be reduced by 1 to have the correct wrap over
		// example: size = 10, Idx = 9 => nextBlocks = 1 as the first Block has only 8 integer
		// example: size = 10, Idx = 18 => nextBlocks = 2 as 8 are in the first block, 9 are in the second block, so Idx = 18 is in the third block
		if ( Idx >= arr[ListPos] ) throw new Exception( "Idx:" + Idx + " is higher than the count:" + arr[ListPos] );
		int blockPos = ListPos;
		int jumpBlocks = ( ( Idx + 1 ) / ( blockSize - 1 ));
		for( int k = 0; k < jumpBlocks; k++ )
		{
			if ( arr[blockPos+blockSize-1] == -1 ) throw new OutOfBlockException(blockPos); 
			blockPos = arr[blockPos+blockSize-1];
		}	
		return blockPos + ( Idx - (jumpBlocks*(blockSize-1)) + 1 );
	}
	public void Add( int ListPos, int NewItem ) throws Exception
	{
		
		int	virtualItemIndex = arr[ListPos]++; // the count is the last new item index 
	    int arrPos; 
		try {
			arrPos = GetArrPos( ListPos, virtualItemIndex );
		}
		catch( OutOfBlockException o )
		{
			int next = ifam.ReserveNextFree();
			arr[ o.lastBlockPos+blockSize-1 ] = next;
			arr[next + blockSize -1 ] = endMarker;
			arrPos = GetArrPos( ListPos, virtualItemIndex );
		}
		arr[arrPos] = NewItem;
	}

	public int GetLength(int ListPos)
	{
		return arr[ListPos];
	}
	
	
	public void Release(int ListPos) throws Exception
	{
		int blockPos = ListPos;
		while ( arr[blockPos + blockSize - 1] != endMarker )
		{
			int nextBlock = arr[blockPos + blockSize - 1];
			ifam.Dispose(blockPos);
			blockPos = nextBlock; 
		}
		ifam.Dispose(blockPos);
	}
}
