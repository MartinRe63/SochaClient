import java.util.LinkedList;
import java.util.List;
import java.util.Random;

// 
// Tree reduction should be done during the TCP communication with the client
//
public class NodeManager {

    static Random r = new Random();
    static double epsilon = (float) 1e-6;
	
	long crake[] = new long[2];
	int[] fishMove;
	long[] visitCnts;
	float[] totalValue;
	int myColor = 0; // 0 = red
	int secondMoveColor = 1;
	int childListPos[];
	int childArr[];	
	IntListManager ilm;
	LongFreeArrayManager lfam;
	
	int firstNode;
	int firstMoveColor = 0; // 0 = red / 1 = blue
	
	boolean stopSelection = false;
	SelectThread thread;
	
	public class SelectThread extends Thread
	{
		 NodeManager nm;
		 public SelectThread( NodeManager Nm )
		 {
			 nm = Nm;
		 }
		 @Override
		 public void run()
		 {
			 try {
				nm.selectAction();
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		 }
	}
	
	public NodeManager( int NodeCount, int MyColor, boolean startSelectThread ) throws Exception
	{
		// values of a node
		fishMove = new int[NodeCount];
		visitCnts = new long[NodeCount];
		totalValue = new float[NodeCount]; 
		childListPos = new int[NodeCount];
		
		// array to store child lists
		childArr = new int[NodeCount*50];
		
		myColor = MyColor;
		ilm = new IntListManager(childArr, 10);
		lfam = new LongFreeArrayManager(visitCnts);
		firstNode = lfam.ReserveNextFree();
	
		if ( startSelectThread )
		{
			thread = new SelectThread(this);
			thread.start();
		}
	}
	public boolean hasChilds( int nodeId ) throws Exception
	{
		if ( lfam.IsFree( nodeId )) throw new Exception("Software Issue - Node is not available.");
		return childListPos[nodeId] != -1;
	}
	public void expandNode( int nodeId, int moveColor, long[][]position ) throws Exception
	{
		if ( lfam.IsFree( nodeId )) throw new Exception("Software Issue - Node is not available.");
		
		// fill the childList with valid moves based on the current position
		
	}
	public double rollOut( int nodeId, int moveColor, long[][]position )
	{
		// calculate the value of this position
		// here to count number of blocks and calculate the block value
		// check if this is the secondMoveColor to check if moveColor will win = 1 or loss = 0
		return 0F;
	}
	public void updateStat( int nodeId, double value )
	{
		visitCnts[nodeId]++;
		totalValue[nodeId] += (float)value;
	}

	private int selectMove( int NodeIdx ) throws Exception
	{
	    int selectedNode = -1;
	    double bestValue = Double.MIN_VALUE;
	    int childList = childListPos[ NodeIdx ];
	    int len = ilm.GetLength(childList);
	    for (int k = 0; k < len; k++ ) {
	    	int childNodeId = ilm.GetItem(childList, k);
	    	double childVisits = visitCnts[childNodeId];
	    	double totValue;
	        double uctValue =
	        		((totValue = totalValue[childNodeId]) / (childVisits + epsilon) +
			Math.sqrt(Math.log(childVisits+1) / (childVisits + epsilon)) +
			r.nextDouble() * epsilon);
	        // small random number to break ties randomly in unexpanded nodes
	        System.out.println("UCT value = " + uctValue + "  tot = " + totValue);
	        if (uctValue > bestValue) {
	            selectedNode = childNodeId;
	            bestValue = uctValue;
	        }
	    }
	    // System.out.println("Returning: " + selected);
	    return selectedNode;
	}
	
	public static void movePosition( int curNode, int[]MoveArr, long[][]positionData, int color) 
	{
		// get the move from the currentPosition and change the positionData
		int[] move = PosManager.getYX(MoveArr[curNode]);
		
		// try to enable register usage
		int moveFrom = move[0];
		int moveTo = move[1];
		
		assert PosManager.IsBit(positionData[color], moveFrom) : "unknown software issue - fish to move is not at the position to move from.";
		assert ! PosManager.IsBit(positionData[color], moveTo ) : "unknown software issue - at the moveto position is a fish of the same color.";
		assert ! PosManager.IsBit(positionData[2], moveTo ) : "unknown software issue - at the moveto position is a crake.";
		
		PosManager.ClearBit(positionData[color], moveFrom );
		PosManager.SetBit(positionData[color], moveTo );
		if ( PosManager.IsBit(positionData[(color+1)%2], moveTo ) )
			PosManager.ClearBit(positionData[(color+1)%2], moveTo );
			
	}
	private static int movePossible( int x, int y, int dir, int lth, long[][]positionData, int color, long[]superlong )
	{
		int newX=0;
		int newY=0;
		int p = y*10+x;
		// goal - use less calculations as possible
		int oppositeColor = (color + 1)  % 2;
		// masks can't check regarding a board extension
		switch ( dir )
		{
			case 0: if ( ( newX = x + lth ) >= 10 ) return -1; 
				newY = y;
				break;
			case 1: if ( ( newX = x + lth ) >= 10 || ( newY = y + lth ) >= 10 ) return -1;
			    break;
			case 2: if ( ( newY = y + lth ) >= 10 ) return -1;
				newX = x;
			    break;
			case 3: if ( ( newX = x - lth ) < 0 || ( newY = y + lth ) >= 10 ) return -1;
			    break;
			case 4: if ( ( newX = x - lth ) < 0 ) return -1;
			    newY = y;
			    break;
			case 5: if ( ( newX = x - lth ) < 0 || ( newY = y - lth ) < 0 ) return -1;
			    break;
			case 6: if ( ( newY = y - lth ) < 0 ) return -1;
				newX = x;
			    break;
			case 7: if ( ( newX = x + lth ) >= 10 || ( newY = y - lth ) < 0 ) return -1;
			    break;
		}
		int newP = newY*10+newX;
		long[] mask = MaskManager.moveMasks[dir][lth-1][p];
		if ( lth > 1 )
		{
			// can't jump over opposite color fishes 
			if ( ( ( mask[0] & positionData[oppositeColor][0] ) != 0 ) || 
				 ( ( mask[1] & positionData[oppositeColor][1] ) != 0 ) )  
			{
				return -1;
			}
		}
		// can't jump over or on crakes - wrong - can jump over crakes
		superlong[0] = MaskManager.moveMasks[dir][lth][p][0];
		superlong[1] = MaskManager.moveMasks[dir][lth][p][1];
		//if ( ( ( maskFull[0] & positionData[2][0] ) != 0 ) || 
        //      ( ( maskFull[1] & positionData[2][1] ) != 0 ) ) 
		//{
		//	return -1;
		//}
		superlong[0] ^= mask[0];
		superlong[1] ^= mask[1];
		// can't jump on my own fish or on crakes
		if ( ( ( superlong[0] & positionData[color][0] ) != 0 ) || 
			 ( ( superlong[1] & positionData[color][1] ) != 0 ) ||
			 ( ( superlong[0] & positionData[2][0] ) != 0 ) || 
		     ( ( superlong[1] & positionData[2][1] ) != 0 ) )  
		{
			return -1;
		}
		return newP;
	}
	public static int getMoveList( long[][]positionData, int color, int[] moves ) throws Exception 
	{
		long mask = 1L;
		int highLong = 0;
		int moveCnt = 0;
		long[] long128 = new long[2];
		for( int p = 0; p < 100; p++ )
		{
			if (mask == 0 ) { mask = 1L; highLong++; }
			long fish = positionData[color][highLong] & mask; 
			if ( fish != 0 )
			{
				int x = p % 10;
				int y = p / 10;
				for ( int dir = 0; dir < 4; dir++ )
				{
					// count blue and red fishes in this direction
					int moveLth = 
						Long.bitCount(MaskManager.directionMasks[dir][p][0] & positionData[0][0]) +
						Long.bitCount(MaskManager.directionMasks[dir][p][1] & positionData[0][1]) + 
						Long.bitCount(MaskManager.directionMasks[dir][p][0] & positionData[1][0]) +
						Long.bitCount(MaskManager.directionMasks[dir][p][1] & positionData[1][1]);
					int newP;
	      			if ( ( newP = movePossible( x, y, dir, moveLth, positionData, color, long128 ) ) > -1 ) 
	      				moves[moveCnt++] = PosManager.PackMove( p, newP);
	      			if ( ( newP = movePossible( x, y, dir+4, moveLth, positionData, color, long128 ) ) > -1 ) 
	      				moves[moveCnt++] = PosManager.PackMove( p, newP);;
				}
			}
			mask <<= 1;
		}
		return moveCnt;
	}
	public void selectAction( ) throws Exception
	{
		int[] visited = new int[20];
		long [][] positionData = new long [3][2];
		while ( ! stopSelection );
		{
			int visitedCnt = 0;
			int cur = firstNode;
			visited[visitedCnt++] = cur;
			int nextMoveColor = firstMoveColor;
			while ( ! ( childListPos[ cur ] == -1 ) ) // ! isLeaf 
			{
				cur = selectMove( cur  );

				// System.out.println("Adding: " + cur);
				visited[visitedCnt++] = cur;                          // visited.add(cur);
				movePosition( cur, fishMove, positionData, nextMoveColor);
				nextMoveColor = ( nextMoveColor + 1 ) % 2;
			}
			expandNode( cur, nextMoveColor, positionData );          //  cur.expand();
	        int newNode = selectMove( cur );                          // TreeNode newNode = cur.select();
	        
	        visited[visitedCnt++] = newNode;                          // visited.add(newNode);
			movePosition( cur, fishMove, positionData, nextMoveColor);
			 

			double value = rollOut ( newNode, nextMoveColor, positionData );
			boolean valueUpdate = true; 
			// int visitedMoveColor = nextMoveColor;
	        for( int k = --visitedCnt; k >= 0; k-- )                  // for (TreeNode node : visited) 
	        {
	            // System.out.println(node);
	        	int visitedNode = visited[k];
	        	updateStat( visitedNode, value);
	        	
	            // node.updateStats(value);
	        	
	            // based on some internet python code values to be added and inverted
	            value = 1-value;
	        }
	        visitedCnt = 0;
		}
	}

	public int getBestMove( ) throws Exception 
	{
		stopSelection = true;
		thread.join();
		if ( lfam.IsFree( firstNode )) throw new Exception("Software Issue - Node is not available.");
		
		int lth = ilm.GetLength(childListPos[firstNode]);
		int ret = -1;
		
		float MaxTotal = 0;
		int bestChildNode = 0;
		
		for( int k = 0; k < lth; k++ ) 
		{
			int childNode = ilm.GetItem(childListPos[firstNode], k);
			if ( lfam.IsFree( childNode )) throw new Exception("Software Issue - Child Node is not available.");
			if ( totalValue[childNode] > MaxTotal ) 
			{
				MaxTotal = totalValue[childNode];
				bestChildNode = childNode;
			}
		}
		
		ret = fishMove[bestChildNode];
		removeFirstNode( bestChildNode );
		firstNode = bestChildNode;
		
		stopSelection = false;
		firstMoveColor = ( firstMoveColor + 1 ) % 2; 
		
		thread = new SelectThread(this);
		thread.start();
		return ret;
	}
	
	public void removeFirstNode( int NewNode )
	{
		
	}
	
	public String getMoveAsString( int ParentNode, int ChildNode )
	{
		return "00->00";
	}
	
	public int[] getMove( int ParentNode, int ChildNode  ) 
	{
		int[] ret = new int [] {0, 0}; 
		return ret;
	}
}
