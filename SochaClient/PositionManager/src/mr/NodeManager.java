package mr;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
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
	// node properties
	int[] fishMove;
	long[] visits;
	float[] totalValue;
	int childListPos[];

	// Array to manage the child lists
	int childArr[];

	
	int moveList[] = new int[16*8];                 // used temporary during node expansion
	long[][][] blockList = new long[2][16][2]; 	        // used temporary during rollout
	// long[][][] blockList1 = new long[2][16][2]; 	    // used temporary during rollout
	int[] blockCnt = new int[2];
	// int[] blockCnt1 = new int[16];
	// long[][] tempPos = new long[3][2];              // temporary Position
			
	IntListManager ilm;
	LongFreeArrayManager lfam;
	
	int firstNode;
	int firstMoveColor = 0; // 0 = red / 1 = blue
	int firstMoveDepth = 0;
	
	long[][] firstPosition;  

	// used to play through the game
	int[] visited = new int[61];
	int visitedCnt = 0;
	long [][] pos = new long [3][2];
	
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
				nm.selectAction(false);
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		 }
	}
	
	private void InitFirstNode( ) throws Exception
	{
		firstNode = lfam.ReserveNextFree();
		initNode(firstNode, 0, 1);
	}
	public NodeManager( int NodeCount, int MyColor, long[][] Position, boolean startSelectThread, int FirstMoveDepth ) throws Exception
	{
		// values of a node
		fishMove = new int[NodeCount];
		visits = new long[NodeCount];
		totalValue = new float[NodeCount]; 
		childListPos = new int[NodeCount];
		
		// array to store child lists
		childArr = new int[NodeCount*10];
		
		firstMoveColor = MyColor;
		ilm = new IntListManager(childArr, 10);
		lfam = new LongFreeArrayManager(visits);
		
		InitFirstNode();
		
		firstPosition = Position;
		firstMoveDepth = FirstMoveDepth;
		
		if ( startSelectThread )
		{
			thread = new SelectThread(this);
			thread.start();
		}
	}
	public boolean hasChilds( int nodeId ) 
	{
		assert ! lfam.IsFree( nodeId ) : "Software Issue - Node is not available.";
		return childListPos[nodeId] != -1;
	}
	public void initNode ( int nodeId, int move, long visitCnt )
	{
		childListPos[nodeId] = -1;
		totalValue[nodeId] = 0.5f;
		visits[nodeId] = visitCnt;
		fishMove[nodeId] = move;
	}
	public void expandNode( int nodeId, int moveColor, long[][]position, int depth ) throws Exception
	{
		// fill the childList with valid moves based on the current position
		assert ! lfam.IsFree( nodeId ) : "Software Issue - Node is not available.";
		assert childListPos[nodeId] == -1 : "Software Issue - Childs available.";
		
		int childListId = childListPos[nodeId] = ilm.ReserveList(); 
        int moveCnt = NodeManager.getMoveList(position, moveColor, moveList);
        for (int i=0; i<moveCnt; i++) {
            int childNodeId;
            ilm.Add( childListId, childNodeId = lfam.ReserveNextFree() ); // children[i] = new TreeNode();
            initNode( childNodeId, moveList[i], depth < 60 ? 1 : Long.MAX_VALUE );
        }
	}
	
	public double rollOut( int nodeId, int color, long[][]pos, int depth ) throws PosManager.GameEndException
	{
		// calculate the value of this position
		// here to count number of blocks and calculate the block value
		// check if this is the secondMoveColor to check if moveColor will win = 1 or loss = 0
		return PosManager.GetValue(pos, color, blockList, blockCnt, depth, firstMoveDepth);
	}
	
	public void updateStat( int nodeId, double value )
	{
		visits[nodeId]++;
		totalValue[nodeId] += (float)value;
	}

	private int selectMove( int NodeIdx ) throws Exception
	{
	    int selectedNode = -1;
	    double bestValue = Double.NEGATIVE_INFINITY;
		IntListManager.IntListIterator i = ilm.getIterator( childListPos[ NodeIdx ] );
	    for (int k = 0; k < i.Lth(); k++ ) {
	    	int childNodeId = i.GetItem(k);
	    	
	    	// childvisits == 0, when the end is reached
	    	long visitCnt = visits[childNodeId];
	        double childVisits = ( visitCnt == Long.MAX_VALUE ? 0 : visitCnt );
	    	double totValue = totalValue[childNodeId];
	    	int move = fishMove[childNodeId];
	    	int moveValue = PosManager.moveValue( move );
	    	double rand = r.nextDouble(); 
	    	double uctValue = 
	        		 totValue / ( childVisits + epsilon ) +
	        		 Math.sqrt( Math.log( childVisits+1 ) / ( childVisits + epsilon ) ) +
	        		 rand * epsilon + moveValue * 10 *  epsilon;
	        
	        // small random number to break ties randomly in unexpanded nodes
	        // System.out.println("UCT value = " + uctValue + "  tot = " + totValue + " " + PosManager.packMoveToString(move));
	        
	        if (uctValue > bestValue) {
	            selectedNode = childNodeId;
	            bestValue = uctValue;
	        }
	    }
	    // System.out.println("Returning: " + selected);
	    assert selectedNode >= 0 : "Child not found.";
	    return selectedNode;
	}
	
	public static void movePosition( int move, long[][]positionData, int color)
	{
		int[]moveDez = PosManager.getYX(move);
		
		// try to enable register usage
		int moveFrom = moveDez[0];
		int moveTo = moveDez[1];
		
		assert PosManager.IsBit(positionData[color], moveFrom) : "unknown software issue - fish to move is not at the position to move from.";
		assert ! PosManager.IsBit(positionData[color], moveTo ) : "unknown software issue - at the moveto position is a fish of the same color.";
		assert ! PosManager.IsBit(positionData[2], moveTo ) : "unknown software issue - at the moveto position is a crake.";
		
		PosManager.ClearBit(positionData[color], moveFrom );
		PosManager.SetBit(positionData[color], moveTo );
		if ( PosManager.IsBit(positionData[(color+1)%2], moveTo ) )
			PosManager.ClearBit(positionData[(color+1)%2], moveTo );
	}
	public static void movePosition( int curNode, int[]MoveArr, long[][]positionData, int color) 
	{
		// get the move from the currentPosition and change the positionData
		movePosition( MoveArr[curNode], positionData, color );
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
	public void selectAction( boolean oneCycle ) throws Exception
	{
		
		while ( ! stopSelection )
		{
			if (oneCycle)
				stopSelection = true;
			visitedCnt = 0;
			int cur = firstNode;
			visited[visitedCnt++] = cur;
			int nextMoveColor = firstMoveColor;
			PosManager.arrayCopy(firstPosition, pos);
			
			while ( ! ( childListPos[ cur ] == -1 ) ) // ! isLeaf 
			{
				cur = selectMove( cur  );

				// System.out.println("Adding: " + cur);
				visited[visitedCnt++] = cur;                          // visited.add(cur);
				movePosition( cur, fishMove, pos, nextMoveColor);
				nextMoveColor = ( nextMoveColor + 1 ) % 2;
			}
			if ( visits[cur] > 0 )
			{
				// expand only if this is no game end node
				expandNode( cur, nextMoveColor, pos, visitedCnt );                    //  cur.expand();
		        cur = selectMove( cur );                        // TreeNode newNode = cur.select();
		        
		        visited[visitedCnt++] = cur;                        // visited.add(newNode);
				movePosition( cur, fishMove, pos, nextMoveColor);
				nextMoveColor = ( nextMoveColor + 1 ) % 2;
			}
			 
			visitedCnt--; // back to the current level it's also showing the current depth
			nextMoveColor = ( nextMoveColor + 1 ) % 2;  // switch also the color back
			
			boolean foundGameEnd = false; 
			double value = 0;
			if ( visits[cur] != 0 )
			{
				try
				{	
					value = rollOut ( cur, nextMoveColor, pos, visitedCnt );
				}
				catch ( PosManager.GameEndException e ) 
				{
					foundGameEnd = true;
					value = e.res;
				}
			}
			else
			{
				value = totalValue[cur];
				foundGameEnd = true;
			}
			// int visitedMoveColor = nextMoveColor;
	        for( int k = visitedCnt; k >= 0; k-- )                  // for (TreeNode node : visited) 
	        {
	            // System.out.println(node);
	        	int visitedNode = visited[k];
	        	if ( foundGameEnd && k == visitedCnt )
	        	{
	        		// System.out.println(PosManager.ToString(pos));
	        		visits[visitedNode] = 0;
	        		totalValue[visitedNode] = (float)value;
	        	}
	        	else
	        	{
	        		updateStat( visitedNode, value);
	        	}
	            // node.updateStats(value);
	        	
	            // based on some internet python code values to be added and inverted
	            value = 1-value;
	        }
	        // mark game end node with visited = -1
		}
		stopSelection = false;
	}

	public int getBestMoveThreadApproach() throws Exception
	{
		stopSelection = true;
		thread.join();

		int ret = getBestMove();
		
		thread = new SelectThread(this);
		thread.start();
		return ret;
	}
	public int getBestMove( ) throws Exception 
	{
		if ( lfam.IsFree( firstNode )) throw new Exception("Software Issue - Node is not available.");
		
		int ret = -1;
		
		float MaxTotal = Float.NEGATIVE_INFINITY;
		int bestChildNode = -1;
		
		IntListManager.IntListIterator i = ilm.getIterator( childListPos[firstNode] );
		for( int k = 0; k < i.Lth(); k++ ) 
		{
			int childNode = i.GetItem( k );
			if ( lfam.IsFree( childNode )) throw new Exception("Software Issue - Child Node is not available.");
			float val = totalValue[childNode] / visits[childNode];
			if ( val > MaxTotal ) 
			{
				MaxTotal = val;
				bestChildNode = childNode;
			}
		}
		
		ret = fishMove[bestChildNode];
		return ret;
	}
	
	private void releaseNode( int NodeId, int NodeIdToExclude ) throws Exception
	{
		int listId;
		if ( ( listId = childListPos[NodeId] ) >= 0 )
		{
			IntListManager.IntListIterator i = ilm.getIterator( listId );
			for( int k = 0; k < i.Lth(); k++ ) 
			{
				int childNodeId = i.GetItem( k );
				if ( childNodeId != NodeIdToExclude )
				{
					releaseNode( childNodeId, NodeIdToExclude );
				}
			}
			ilm.Release(childListPos[NodeId]);
			childListPos[NodeId] = -1;
		}
		else if ( NodeId != NodeIdToExclude )
		{
			lfam.Dispose( NodeId );
		}
	}
	
	private int findNode ( int nodeId, int move ) throws Exception
	{
		int ret = -1;
		IntListManager.IntListIterator i = ilm.getIterator( childListPos[nodeId] );
		for( int k = 0; k < i.Lth(); k++ ) 
		{
			int childNodeId = i.GetItem(k);
			if ( fishMove[ childNodeId ] == move )
			{
				return childNodeId;
			}
		}		
		return ret;
	}
	
	public void DisposeTree ( int move1, int move2 ) throws Exception
	{
		int nodeId = findNode( firstNode, move1 );
		assert nodeId >= 0 : "The first move must be a child of the first Node.";
		// movePosition(nodeId, fishMove, pos, firstMoveColor );
		
		nodeId = findNode( nodeId, move2 );
		// if ( nodeId >= 0 )
		// movePosition(nodeId, fishMove, pos, (firstMoveColor+1)%2 );
		int nodeCount = lfam.reservedBlockCnt;
		releaseNode( firstNode, nodeId );
		firstNode = nodeId; 
		if ( firstNode < 0 )
		{
			InitFirstNode();
		}
		firstMoveDepth+=2;
	}
	
	public int BestMove() throws Exception
	{
		return getBestMove();
	}
	
	
	class ToSort implements Comparable
	{
		private float totValue;
		private long visits;
		private int move;
		private double value()
		{
			return totValue / visits;
		}
		public ToSort( float TotalValue, long Visits, int Move )
		{
			totValue = TotalValue; 
			visits = Visits; 
			move = Move;
		}
		@Override
		public int compareTo(Object arg0) {
			// TODO Auto-generated method stub
			return value() > ((ToSort)arg0).value() ? 1 : 0;
		}
		@Override
		public String toString() {
			return PosManager.packMoveToString(move) + ":" + value() + " v:" + visits + "\r\n";
		}
		
		
	}
	public String ValuesToString( ) throws Exception
	{
		IntListManager.IntListIterator i = ilm.getIterator( childListPos[firstNode] );
		List<ToSort> l = new ArrayList<ToSort>();
		for( int k = 0; k < Integer.min( 10, i.Lth() ) ; k++ ) 
		{
			int idx = i.GetItem( k );
			if ( visits[idx] > 1 )
				l.add( new ToSort( totalValue[idx], visits[idx], fishMove[idx]));
		}
		l.sort( Comparator.comparing( a -> a.value() * -1 )  );
		String res = "";
		for ( ToSort val : l )
		{
			res += val;
		}
		return res;
	}
	
	public String LastPositionToString( )
	{
		return ToString() + "\r\n" + PosManager.ToString(pos);
	}
	
	public String ToString()
	{
		return "Last Value=" + Float.toString( totalValue[ visited[visitedCnt]] ) + " Depth=" + visitedCnt;
	}
	public int[] getMove( int ParentNode, int ChildNode  ) 
	{
		int[] ret = new int [] {0, 0}; 
		return ret;
	}
	
}
