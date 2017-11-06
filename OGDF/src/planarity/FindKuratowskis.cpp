/*
 * $Revision: 1.2 $
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2007-11-09 12:12:41 +0100 (Fr, 09 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief implementation of the class FindKuratowskis
 * 
 * \author Jens Schmidt
 * 
 * \par License:
 * This file is part of the Open Graph Drawing Framework (OGDF).
 * Copyright (C) 2005-2007
 * 
 * \par
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 or 3 as published by the Free Software Foundation
 * and appearing in the files LICENSE_GPL_v2.txt and
 * LICENSE_GPL_v3.txt included in the packaging of this file.
 *
 * \par
 * In addition, as a special exception, you have permission to link
 * this software with the libraries of the COIN-OR Osi project
 * (http://www.coin-or.org/projects/Osi.xml), all libraries required
 * by Osi, and all LP-solver libraries directly supported by the
 * COIN-OR Osi project, and distribute executables, as long as
 * you follow the requirements of the GNU General Public License
 * in regard to all of the software in the executable aside from these
 * third-party libraries.
 * 
 * \par
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * \par
 * You should have received a copy of the GNU General Public 
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 * 
 * \see  http://www.gnu.org/copyleft/gpl.html
 ***************************************************************/


#include <ogdf/internal/planarity/FindKuratowskis.h>
#include <ogdf/basic/simple_graph_alg.h>

#ifdef FILE_OUTPUT
	#include <Output.h>
#endif

namespace ogdf {


// copy pointer of class Kuratowski
void KuratowskiStructure::copyPointer(const KuratowskiStructure& orig, SListPure<WInfo>& list) {
	SListConstIterator<SListPure<adjEntry> > itHighOrig=orig.highestXYPaths.begin();
	SListIterator<SListPure<adjEntry> > itHigh=highestXYPaths.begin();
	SListConstIterator<SListPure<adjEntry> > itZOrig=orig.zPaths.begin();
	SListIterator<SListPure<adjEntry> > itZ=zPaths.begin();
	SListConstIterator<ExternE> itExternStartOrig=orig.externE.begin();
	SListIterator<ExternE> itExternStart=externE.begin();
	SListConstIterator<ExternE> itExternEndOrig=orig.externE.begin();
	SListIterator<ExternE> itExternEnd=externE.begin();
	SListIterator<WInfo> it;
	for (it=list.begin(); it.valid(); ++it) {
		WInfo& info(*it);
		if (info.highestXYPath!=NULL) {
			// go to referenced object
			while (info.highestXYPath != &(*itHighOrig)) {
				++itHigh;
				++itHighOrig;
			}
			OGDF_ASSERT(itHigh.valid() && itHighOrig.valid());
			info.highestXYPath=&(*itHigh);
		}
		if (info.zPath!=NULL) {
			// go to referenced object
			while (info.zPath != &(*itZOrig)) {
				++itZ;
				++itZOrig;
			}
			OGDF_ASSERT(itZ.valid() && itZOrig.valid());
			info.zPath=&(*itZ);
		}
		if (info.externEStart.valid()) {
			// go to referenced object
			while ((*info.externEStart).theNode != (*itExternStartOrig).theNode) {
				++itExternStartOrig;
				++itExternStart;
			}
			OGDF_ASSERT(itExternStartOrig.valid() && itExternStart.valid());
			info.externEStart = itExternStart;
		}
		if (info.externEEnd.valid()) {
			// go to referenced object
			while ((*info.externEEnd).theNode != (*itExternEndOrig).theNode) {
				++itExternEndOrig;
				++itExternEnd;
			}
			OGDF_ASSERT(itExternEndOrig.valid() && itExternEnd.valid());
			info.externEEnd = itExternEnd;
		}
	}
}

// copy class Kuratowski
void KuratowskiStructure::copy(const KuratowskiStructure& orig) {
	V = orig.V;
	V_DFI = orig.V_DFI;
	R = orig.R;
	RReal = orig.RReal;
	stopX = orig.stopX;
	stopY = orig.stopY;
	
	wNodes = orig.wNodes;
	highestFacePath = orig.highestFacePath;
	highestXYPaths = orig.highestXYPaths;
	externalFacePath = orig.externalFacePath;
	externalSubgraph = orig.externalSubgraph;
	pertinentSubgraph = orig.pertinentSubgraph;
	zPaths = orig.zPaths;
	externE = orig.externE;
	stopXStartnodes = orig.stopXStartnodes;
	stopYStartnodes = orig.stopYStartnodes;
	stopXEndnodes = orig.stopXEndnodes;
	stopYEndnodes = orig.stopYEndnodes;
	
	// copy pointer
	copyPointer(orig,wNodes);
}

// clears members
void KuratowskiStructure::clear() {
	V=R=RReal=stopX=stopY=NULL;
	V_DFI = 0;
  	wNodes.clear();
  	highestFacePath.clear();
  	highestXYPaths.clear();
  	externalFacePath.clear();
  	externalSubgraph.clear();
  	pertinentSubgraph.clear();
  	zPaths.clear();
  	externE.clear();
  	stopXStartnodes.clear();
  	stopYStartnodes.clear();
  	stopXEndnodes.clear();
  	stopYEndnodes.clear();
}

// class FindKuratowski
FindKuratowskis::FindKuratowskis(BoyerMyrvoldPlanar* bm)
	  : pBM(bm),
	  	m_g(bm->m_g),
	  	m_embeddingGrade(bm->m_embeddingGrade),
	  	
	  	#ifdef FILE_OUTPUT
	  	m_ga(bm->m_ga),
	  	#endif
	  	
	  	m_bundles(bm->m_bundles),
	  	
		#ifdef OGDF_DEBUG
		nirvana(bm->nirvana), // if not in statistics mode, delete all statistics
		#endif
		
		// initialize Members of BoyerMyrvoldPlanar
		m_realVertex(bm->m_realVertex),
		m_dfi(bm->m_dfi),
		m_nodeFromDFI(bm->m_nodeFromDFI),
		m_link(bm->m_link),
		m_adjParent(bm->m_adjParent),
		m_leastAncestor(bm->m_leastAncestor),
		m_edgeType(bm->m_edgeType),
		m_lowPoint(bm->m_lowPoint),
		m_highestSubtreeDFI(bm->m_highestSubtreeDFI),
		m_separatedDFSChildList(bm->m_separatedDFSChildList),
		m_pointsToRoot(bm->m_pointsToRoot),
		m_visitedWithBackedge(bm->m_visitedWithBackedge),
		m_backedgeFlags(bm->m_backedgeFlags),
		m_pertinentRoots(bm->m_pertinentRoots)
{
	OGDF_ASSERT(bm != NULL);
	m_nodeMarker = 0;
};

// finds root node of the bicomp containing the stopping node stopX
node FindKuratowskis::findRoot(node stopX) {
	int dir = CCW;
	while (m_realVertex[stopX]==NULL)
		stopX = pBM->successorWithoutShortCircuit(stopX,dir);
	return stopX;
}

// extracts highest face path (contains all highest xy-paths)
void FindKuratowskis::extractHighestFacePath(
				ListPure<adjEntry>& highestFacePath,
				int marker) {
	adjEntry adj = pBM->beforeShortCircuitEdge(k.R,CCW);
	adjEntry end = pBM->beforeShortCircuitEdge(k.R,CW);
	node x,target;
	while (adj != end->twin()) {
		x = adj->theNode();
		
		#ifdef FILE_OUTPUT
		m_ga.colorNode(x)="#BB00BB";
		#endif
		if (m_wasHere[x] >= marker) {
			// node is already visited on facepath: pop until duplicate node found
			OGDF_ASSERT(!highestFacePath.empty());
			while (highestFacePath.back()->theNode()!=x) highestFacePath.popBack();
			// sign cut-vertex with marker+1
			m_wasHere[x] = marker+1;
		} else {
			highestFacePath.pushBack(adj);
			// sign visited nodes with marker
			m_wasHere[x] = marker;
		}
		
		do {
			adj = adj->cyclicSucc();
			target = adj->twin()->theNode();
			if (target == k.R) m_wasHere[x] = marker+1;
		} while (adj != end &&
				(m_edgeType[adj->theEdge()] == EDGE_BACK_DELETED ||
				m_dfi[target] <= m_dfi[k.R]));
		adj = adj->twin();
	}
}

// extract external facepath in direction CCW and split the highest facepath
// in highest xy-paths. marker marks the node, highMarker is used to check,
// whether the node was visited before by the highest facepath traversal.
// highMarker+1 identifies the nodes that are zNodes.
void FindKuratowskis::extractExternalFacePath(
				SListPure<adjEntry>& externalFacePath,
				const ListPure<adjEntry>& highestFacePath,
				int marker,
				int highMarker)
{
	int dir = CCW;
	// x traverses the external facepath
	node x = pBM->successorWithoutShortCircuit(k.R,dir);
	externalFacePath.pushBack(pBM->beforeShortCircuitEdge(k.R,CCW));
	m_wasHere[k.R] = marker;
	while (x != k.R) {
		// set visited sign on nodes that are both on the highest and external facepath
		if (m_wasHere[x]>=highMarker) m_wasHere[x] = marker;
		externalFacePath.pushBack(pBM->beforeShortCircuitEdge(x,dir));
		x = pBM->successorWithoutShortCircuit(x,dir);
	}
	
	dir = CCW;
	x = pBM->successorWithoutShortCircuit(k.R,dir);
	ListConstIterator<adjEntry> highIt = highestFacePath.begin();
	OGDF_ASSERT(x == (*highIt)->theNode());
	SListPure<adjEntry> XYPathList;
	SListPure<adjEntry> zList;
	WInfo info;
	adjEntry adj = pBM->beforeShortCircuitEdge(k.R,CCW);
	adjEntry temp;
	while (x != k.R) {
		// go along the highest face path until next visited sign
		OGDF_ASSERT(adj->theNode()==x);
		if (m_wasHere[x] == marker) {
			XYPathList.clear();
			zList.clear();
			info.w = NULL;
			info.minorType = 0;
			info.highestXYPath = NULL;
			info.zPath = NULL;
			info.pxAboveStopX = false;
			info.pyAboveStopY = false;
			info.externEStart = NULL;
			info.externEEnd = NULL;
			info.firstExternEAfterW = NULL;
		}
		
		// push in wNodes-list
		if (pBM->pertinent(x)) {
			info.w = x;
			k.wNodes.pushBack(info);
		}
		
		// compute next highestXYPath
		if (m_wasHere[x] == marker &&
					m_wasHere[pBM->constSuccessorWithoutShortCircuit(x,dir)] != marker) {
			// traverse highFacePath to x
			while ((*highIt)->theNode() != x) ++highIt;
			OGDF_ASSERT(highIt.valid());
			XYPathList.pushBack(adj);
			OGDF_ASSERT((*highIt.succ())->theNode() !=
						pBM->constSuccessorWithoutShortCircuit(x,dir));
			
			// traverse highFacePath to next marker
			do {
				++highIt;
				if (!highIt.valid()) break;
				temp = *highIt;
				XYPathList.pushBack(temp);
				// check, if node is z-node and push one single z-node
				if (m_wasHere[temp->theNode()]==highMarker+1 && zList.empty())
					zList.pushBack(temp);
			} while (m_wasHere[temp->theNode()] != marker);
			
			// save highestXY-Path
			OGDF_ASSERT(!XYPathList.empty());
			k.highestXYPaths.pushBack(XYPathList);
			info.highestXYPath = &k.highestXYPaths.back();
			
			// compute path from zNode to V and save it
			if (!zList.empty()) {
				OGDF_ASSERT(zList.size()==1); // just one zNode for now
				temp = zList.back();
				do {
					do {
						temp = temp->cyclicSucc();
						OGDF_ASSERT(m_dfi[temp->twin()->theNode()]==m_dfi[k.R] ||
									m_dfi[temp->twin()->theNode()]>=m_dfi[k.RReal]);
					} while (m_edgeType[temp->theEdge()]==EDGE_BACK_DELETED);
					temp = temp->twin();
					zList.pushBack(temp);
				} while (temp->theNode() != k.R);
				k.zPaths.pushBack(zList);
				info.zPath = &k.zPaths.back();
			}
		}
		
		// go on
		adj = pBM->beforeShortCircuitEdge(x,dir);
		x = pBM->successorWithoutShortCircuit(x,dir);
	}
}

// separate pertinent nodes in the lists of possible different minor-types
void FindKuratowskis::splitInMinorTypes(
			const SListPure<adjEntry>& externalFacePath,
			int marker)
{
	// mark nodes, which are before stopX or behind stopY in CCW-traversal and add
	// all extern nodes strictly between stopX and stopY to list
	// externE for minor E (pertinent nodes are considered because of the
	// position of z left or right of w)
	SListConstIterator<adjEntry> itExtern;
	SListIterator<WInfo> it = k.wNodes.begin();
	node x;
	bool between = false;
	SListPure<WInfo*> infoList;
	SListIterator<WInfo*> itList;
	ExternE externEdummy;
	// compute list of externE nodes
	for (itExtern=externalFacePath.begin(); itExtern.valid(); ++itExtern) {
		x = (*itExtern)->theNode();
		if (x==k.stopX || x==k.stopY) {
			between = (between==false) ? true : false;
		} else {
			if (!between) {
				m_wasHere[x]=marker;
			} else {
				if (pBM->externallyActive(x,k.V_DFI)) {
					externEdummy.theNode = x;
					
					// check minor type B and save extern linkage
					if (it.valid() && (*it).w==x &&
							!m_pertinentRoots[x].empty() &&
							m_lowPoint[m_nodeFromDFI[-m_dfi[m_pertinentRoots[x].back()]]]
							< k.V_DFI) {
						WInfo& info(*it);
						
						// checking minor type B
						info.minorType |= WInfo::B;
						// mark extern node for later extraction
						externEdummy.startnodes.pushBack(0);
						// create externE-list
						k.externE.pushBack(externEdummy);
						// save extern linkage
						info.externEStart = k.externE.rbegin();
						info.externEEnd = k.externE.rbegin();
					} else {
						// create externE-list
						externEdummy.startnodes.clear();
						k.externE.pushBack(externEdummy);
					}
					
					// save for each wNode the first externally active successor
					// on the external face
					for (itList = infoList.begin(); itList.valid(); ++itList)
						(*itList)->firstExternEAfterW = x;
					infoList.clear();
					
					
				}
				
				// get appropriate WInfo
				if (it.valid() && (*it).w==x) {
					infoList.pushBack(&(*it));
					++it;
				}
			}
		}
	}
	
	// divide wNodes in different minor types
	// avoids multiple computation of the externE range
	itExtern = externalFacePath.begin();
	SListIterator<ExternE> itExternE = k.externE.begin();
	WInfo* oldInfo = NULL;
	for (it=k.wNodes.begin(); it.valid(); ++it) {
		WInfo& info(*it);
		
		// checking minor type A
		if (k.RReal!=k.V) info.minorType |= WInfo::A;
		
		// if a XYPath exists
		if (info.highestXYPath!=NULL) {
			if (m_wasHere[info.highestXYPath->front()->theNode()]==marker)
				info.pxAboveStopX = true;
			if (m_wasHere[info.highestXYPath->back()->theNode()]==marker)
				info.pyAboveStopY = true;
			
			// checking minor type C
			if (info.pxAboveStopX || info.pyAboveStopY)
				info.minorType |= WInfo::C;
			
			// checking minor type D
			if (info.zPath!=NULL) info.minorType |= WInfo::D;
			
			// checking minor type E
			if (!k.externE.empty()) {
				node t;
				
				// compute valid range of externE-nodes in linear time
				if (oldInfo!=NULL && info.highestXYPath==oldInfo->highestXYPath) {
					// found the same highestXYPath as before
					info.externEStart = oldInfo->externEStart;
					info.externEEnd = oldInfo->externEEnd;
					if (oldInfo->minorType & WInfo::E) info.minorType |= WInfo::E;
				} else {
					// compute range of a new highestXYPath
					node px;
					if (info.pxAboveStopX) px = k.stopX;
						else px = info.highestXYPath->front()->theNode();
					node py;
					if (info.pyAboveStopY) py = k.stopY;
						else py = info.highestXYPath->back()->theNode();
					while ((*itExtern)->theNode() != px) ++itExtern;
					t = (*(++itExtern))->theNode();
					node start = NULL;
					node end = NULL;
					while (t != py) {
						if (pBM->externallyActive(t,k.V_DFI)) {
							if (start==NULL) start = t;
							end = t;
						}
						t = (*(++itExtern))->theNode();
					}
					if (start != NULL) {
						while ((*itExternE).theNode != start) ++itExternE;
						info.externEStart = itExternE;
						// mark node to extract external subgraph later
						(*itExternE).startnodes.pushBack(0);
						node temp = start;
						while (temp != end) {
							temp = (*++itExternE).theNode;
							// mark node to extract external subgraph later
							(*itExternE).startnodes.pushBack(0);
						}
						info.externEEnd = itExternE;
						info.minorType |= WInfo::E;
					}
					oldInfo = &info;
				}
			}
		}
		
		/*
		// use this to find special kuratowski-structures
		if ((info.minorType & (WInfo::A|WInfo::B|WInfo::C|WInfo::D|WInfo::E)) ==
			(WInfo::A|WInfo::B|WInfo::C|WInfo::D|WInfo::E)) {
			char t; cin >> t;
		}
		*/
	}
	
	// extract the externalSubgraph of all saved externally active nodes
	// exclude the already extracted minor b-types
	#ifdef OGDF_DEBUG
	int visited = m_nodeMarker+1;
	#endif
	for (itExternE=k.externE.begin(); itExternE.valid(); ++itExternE) {
		if ((*itExternE).startnodes.empty()) continue;
		
		ExternE& externE(*itExternE);
		externE.startnodes.clear();
		#ifdef STATISTICS
		stat << "\nexternalSubgraph of minortype B or E on node " << m_dfi[externE.theNode];
		stat << ":";
		#endif
		if (m_bundles) {
			OGDF_ASSERT(m_wasHere[externE.theNode] < visited);
			extractExternalSubgraphBundles(externE.theNode,k.V_DFI,
										k.externalSubgraph,++m_nodeMarker);
		} else {
			extractExternalSubgraph(externE.theNode,k.V_DFI,externE.startnodes,
															externE.endnodes);
			SListIterator<int> itInt;
			SListPure<edge> dummy;
			for (itInt = externE.startnodes.begin(); itInt.valid(); ++itInt)
				externE.externalPaths.pushBack(dummy);
		}
	}
}

// extracts and adds external subgraph from stopnode to ancestors of the node with dfi root
// to edgelist, nodeMarker is used as a visited flag. returns the endnode with lowest dfi.
void FindKuratowskis::extractExternalSubgraph(
			const node stop,
			int root,
			SListPure<int>& externalStartnodes,
			SListPure<node>& externalEndnodes)
{
	int lowpoint;
	ListConstIterator<node> it;
	
	#ifdef STATISTICS
	stat << " Endnodes: ";
	#endif
	if (m_leastAncestor[stop] < root) {
		externalStartnodes.pushBack(m_dfi[stop]);
		externalEndnodes.pushBack(m_nodeFromDFI[m_leastAncestor[stop]]);
		#ifdef STATISTICS
		stat << "(" << m_dfi[stop] << "," << m_leastAncestor[stop] << ") ";
		#endif
	}
	
	// descent to external active child bicomps of stopnode
	node temp;
	for (it = m_separatedDFSChildList[stop].begin(); it.valid(); ++it) {
		temp = *it;
		lowpoint = m_lowPoint[temp];
		if (lowpoint >= root) break;
		
		externalStartnodes.pushBack(m_dfi[temp]);
		externalEndnodes.pushBack(m_nodeFromDFI[lowpoint]);
		#ifdef STATISTICS
		stat << "(" << m_dfi[temp] << "," << lowpoint << ") ";
		#endif
	}
}

// extract and add external subgraph from stopnode to ancestors of the node with dfi root
// to edgelist, nodeMarker is used as a visited flag. returns the endnode with lowest dfi.
void FindKuratowskis::extractExternalSubgraphBundles(
			const node stop,
			int root,
			SListPure<edge>& externalSubgraph,
			int nodeMarker)
{
	node v,temp;
	adjEntry adj;
	
	#ifdef OGDF_DEBUG
	forall_nodes(v,m_g) OGDF_ASSERT(m_wasHere[v]!=nodeMarker);
	#endif
	
	StackPure<node> stack; // stack for dfs-traversal
	ListConstIterator<node> it;
	stack.push(stop);
	while (!stack.empty()) {
		v = stack.pop();
		if (m_wasHere[v]==nodeMarker) continue;
		// mark visited nodes
		m_wasHere[v]=nodeMarker;
		
		// ONLY STATISTICS
		#ifdef STATISTICS
		stat << " " << m_dfi[v];
		#endif
		
		// search for unvisited nodes and add them to stack
		forall_adj(adj,v) {
			temp = adj->twin()->theNode();
			if (m_edgeType[adj->theEdge()]==EDGE_BACK_DELETED) continue;
			
			// go along backedges to ancestor (ignore virtual nodes)
			if (m_dfi[temp] < root && m_dfi[temp] > 0) {
				OGDF_ASSERT(m_edgeType[adj->theEdge()]==EDGE_BACK);
				externalSubgraph.pushBack(adj->theEdge());
				#ifdef STATISTICS
				stat << "[" << m_dfi[temp] << "]";
				#endif
			} else if (v != stop && m_dfi[temp]>=m_dfi[v]) {
				// set flag and push unvisited nodes
				OGDF_ASSERT(m_edgeType[adj->theEdge()]==EDGE_BACK ||
							m_edgeType[adj->theEdge()]==EDGE_DFS ||
							m_edgeType[adj->theEdge()]==EDGE_BACK_DELETED);
				externalSubgraph.pushBack(adj->theEdge());
				if (m_wasHere[temp] != nodeMarker) stack.push(temp);
			}
		}
		
		// descent to external active child bicomps
		for (it = m_separatedDFSChildList[v].begin(); it.valid(); ++it) {
			temp = *it;
			if (m_lowPoint[temp] >= root) break;
			#ifdef STATISTICS
			stat << "(" << m_dfi[m_nodeFromDFI[-m_dfi[temp]]] << ")";
			#endif
			stack.push(m_nodeFromDFI[-m_dfi[temp]]);
		}
	}
}

// extract pertinent paths from all w-nodes to v to edgelist
void FindKuratowskis::extractPertinentSubgraph(
				SListPure<WInfo>& W_All,
				const node V)
{
	SListPure<edge> path;
	SListIterator<WInfo> it;
	adjEntry adj;
	int minDFI = -m_dfi[k.R];
	int maxDFI = m_highestSubtreeDFI[m_nodeFromDFI[minDFI]];
	int targetDFI;
	node target;
	edge e;
	
	#ifdef STATISTICS
	stat << "\npertinentSubgraph without bundles between DFIs ";
	stat << minDFI << " and " << maxDFI << ": ";
	#endif
	
	// create links from pertinent nodes to WInfo
	for (it = W_All.begin(); it.valid(); ++it)
		m_getWInfo[(*it).w] = &(*it);
	
	// add all pertinent paths to WInfo
	forall_adj(adj,k.V) {
		if (m_edgeType[adj->theEdge()]==EDGE_BACK_DELETED) continue;
		targetDFI = m_dfi[adj->twin()->theNode()];
		if (targetDFI >= minDFI && targetDFI <= maxDFI) {
			// target node is in subtree of a pertinent node
			#ifdef STATISTICS
			stat << "\n\t" << k.V_DFI << "->" << targetDFI << flush;
			#endif
			
			// delete last edge and backedgeFlags
			target = adj->twin()->theNode();
			e = adj->theEdge();
			path.pushFront(e);
			OGDF_ASSERT(!m_backedgeFlags[target].empty());
			m_backedgeFlags[target].clear();
			m_edgeType[e] = EDGE_BACK_DELETED;
			#ifdef FILE_OUTPUT
			m_ga.colorEdge(e) = "#AF00F0";
			#endif
			// delete backedge-counter on virtual root node
			--m_visitedWithBackedge[m_pointsToRoot[e]];
			OGDF_ASSERT(m_visitedWithBackedge[m_pointsToRoot[e]] >= 0);
			#ifdef STATISTICS
			if (m_visitedWithBackedge[m_pointsToRoot[e]] == 0)
				stat << "(this was the last backedge)";
			#endif
			
			// go up along the DFS-path
			while (m_getWInfo[target] == NULL) {
				path.pushFront(m_adjParent[target]->theEdge());
				target = m_adjParent[target]->theNode();
				if (m_realVertex[target] != NULL) {
					target = m_realVertex[target];
					m_pertinentRoots[target].clear();
				}
				#ifdef STATISTICS
				stat << "->" << m_dfi[target];
				#endif
			}
			
			// save path
			m_getWInfo[target]->pertinentPaths.pushBack(path);
			path.clear();
		}
	}
	
	// delete links from pertinent nodes to WInfo
	for (it = W_All.begin(); it.valid(); ++it)
		m_getWInfo[(*it).w] = NULL;
}


// extract and add pertinent subgraph from all w-nodes to v to edgelist
void FindKuratowskis::extractPertinentSubgraphBundles(
				const SListPure<WInfo>& W_All,
				const node V,
				SListPure<edge>& pertinentSubgraph,
				int nodeMarker)
{
	node w,x;
	adjEntry adj;
	edge e;
	
	#ifdef OGDF_DEBUG
	forall_nodes(w,m_g) OGDF_ASSERT(m_wasHere[w]!=nodeMarker);
	#endif
	
	StackPure<node> stack; // stack for dfs-traversal
	SListIterator<node> it;
	SListConstIterator<WInfo> itt;
	// for all w-nodes
	for (itt = W_All.begin(); itt.valid(); ++itt) {
		node currentWNode = (*itt).w;
		stack.push(currentWNode);
		
		#ifdef STATISTICS
		stat << "\npertinentSubgraph w-node " << m_dfi[currentWNode] << ": ";
		#endif
		
		// until stack is empty, do dfs-traversal in bicomps and descent to
		// pertinent child bicomps
		while (!stack.empty()) {
			w = stack.pop();
			if (m_wasHere[w]==nodeMarker) continue;
			// mark visited nodes
			m_wasHere[w]=nodeMarker;
			
			// ONLY STATISTICS
			#ifdef STATISTICS
			stat << " " << m_dfi[w];
			#endif
			
			// search for unvisited nodes and add them to stack
			forall_adj(adj,w) {
				e = adj->theEdge();
				if (m_edgeType[e]==EDGE_BACK_DELETED) continue;
				x = adj->twin()->theNode();
				
				// go along pertinent backedges to V (ignore virtual nodes)
				if (x==V && m_edgeType[e]!=EDGE_BACK_DELETED) {
					OGDF_ASSERT(m_edgeType[e]==EDGE_BACK);
					// delete edge and delete backedgeFlags
					m_edgeType[e] = EDGE_BACK_DELETED;
					m_backedgeFlags[w].clear();
					// delete backedge-counter on virtual root node
					--m_visitedWithBackedge[m_pointsToRoot[e]];
					OGDF_ASSERT(m_visitedWithBackedge[m_pointsToRoot[e]]>=0);
					pertinentSubgraph.pushBack(e);
					#ifdef STATISTICS
					stat << "[" << m_dfi[x] << "]";
					if (m_visitedWithBackedge[m_pointsToRoot[e]]==0)
						stat << ",this was the last backedge,";
					#endif
					#ifdef FILE_OUTPUT
					m_ga.colorEdge(e) = "#AF00F0";
					#endif
					
				} else if (w != currentWNode && m_dfi[x] >= m_dfi[w]) {
					OGDF_ASSERT(m_edgeType[adj->theEdge()]==EDGE_DFS ||
								m_edgeType[adj->theEdge()]==EDGE_BACK ||
								m_edgeType[adj->theEdge()]==EDGE_BACK_DELETED);
					// set flag and push unvisited nodes
					pertinentSubgraph.pushBack(e);
					if (m_wasHere[x] != nodeMarker) stack.push(x);
				}
			}
			
			// descent to pertinent child bicomps
			for (it = m_pertinentRoots[w].begin(); it.valid(); ++it) {
				#ifdef STATISTICS
				stat << "(" << m_dfi[*it] << ")";
				#endif
				stack.push(*it);
			}
			// delete all pertinentRoots-lists, since there are no pertinent backedges any more
			m_pertinentRoots[w].clear();
		}
	}
}

// add Kuratowski structure on current node V
void FindKuratowskis::addKuratowskiStructure(
						const node currentNode,
						const node root,
						const node stopx,
						const node stopy)
{
	OGDF_ASSERT(currentNode!=NULL && root!=NULL && stopx!=NULL && stopy!=NULL);
	OGDF_ASSERT(stopx!=stopy && currentNode!=stopx && currentNode!=stopy && m_dfi[root]<0);
	OGDF_ASSERT(!pBM->pertinent(stopx) &&
				pBM->externallyActive(stopx,m_dfi[currentNode]));
	OGDF_ASSERT(!pBM->pertinent(stopy) &&
				pBM->externallyActive(stopy,m_dfi[currentNode]));
	OGDF_ASSERT(findRoot(stopx)==root); // check root
	OGDF_ASSERT(pBM->wNodesExist(root,stopx,stopy));
	OGDF_ASSERT(isSimpleUndirected(m_g)); // Graph has to be simple
	OGDF_ASSERT(m_embeddingGrade > BoyerMyrvoldPlanar::doNotFind);
	// check, if we have found enough kuratowski structures
	OGDF_ASSERT(m_embeddingGrade <= 0 || allKuratowskis.size() < m_embeddingGrade);
	
	// init NodeArrays in first invocation
	if (!m_wasHere.valid()) {
		if (!m_bundles) {
			OGDF_ASSERT(!m_getWInfo.valid() && m_getWInfo.graphOf()==NULL);
			m_getWInfo.init(m_g,NULL);
		}
		OGDF_ASSERT(m_wasHere.graphOf()==NULL);
		m_wasHere.init(m_g,0);
	}
	
	// delete old KuratowskiStruture and initialize new structure
  	k.clear();
  	k.V = currentNode;
  	k.V_DFI = m_dfi[currentNode];
	k.stopX = stopx;
	k.stopY = stopy;
	k.R = root;
	k.RReal = m_realVertex[k.R];
  	
  	#ifdef STATISTICS
	stat << "\n\n\n------------------------------------------------------------";
	stat << "\n\tfind " << allKuratowskis.size()+1
		<< ". kuratowski structure (on node " << k.V_DFI << ")";
	stat << "\n------------------------------------------------------------";
	#endif
	
	// flip bicomp with root R with or without reversed flipping. changes the embedding
	// process completely.
	pBM->flipBicomp(-m_dfi[k.R],++m_nodeMarker,m_wasHere,false,true);
//	pBM->flipBicomp(-m_dfi[k.R],++m_nodeMarker,m_wasHere,false,false);
	
	#ifdef STATISTICS
	stat << "\n\nV=" << k.V_DFI;
	if (m_bundles) {
		stat << "\nextract Bundles";
	} else stat << "\nextract no Bundles";
	stat << "\nNot embedded backedges: ";
	node x;
	adjEntry adj;
	forall_adj(adj,k.V) {
		x=adj->twin()->theNode();
		if (m_dfi[x] > k.V_DFI && m_edgeType[adj->theEdge()]==EDGE_BACK)
			stat << k.V_DFI << "->" << m_dfi[x] << " ";
	}
	stat << "\nother pertinentRoots of V: ";
	SListIterator<node> it;
	for (it=m_pertinentRoots[k.V].begin();it.valid();++it)
		stat << m_dfi[*it] << ", ";
	stat << "\n\nR=" << m_dfi[k.R]; // virtual root
	stat << "\nstopX=" << m_dfi[k.stopX];
	stat << "\nstopY=" << m_dfi[k.stopY];
	#endif
	
	// extract highest facepath (contains all highest xy-paths)
	extractHighestFacePath(k.highestFacePath,++m_nodeMarker);
	++m_nodeMarker;
	
	#ifdef STATISTICS
	stat << "\n\nhighest facepath: " << flush;
	ListIterator<adjEntry> itt;
	for (itt=k.highestFacePath.begin(); itt.valid(); ++itt)
		stat << m_dfi[(*itt)->theNode()]
			<< (m_wasHere[(*itt)->theNode()]-m_nodeMarker+1==1 ? "(z)," : ",");
	#endif
	
	// extract external facepath in direction CCW and split the highest facepath in
	// highest xy-paths
	++m_nodeMarker;
	extractExternalFacePath(k.externalFacePath,k.highestFacePath,m_nodeMarker,
															m_nodeMarker-2);
	
	// extract external subgraph from stopX and stopY to ancestors of R
	#ifdef STATISTICS
	stat << "\nexternalSubgraph X:" << flush;
	#endif
	if (m_bundles) {
		extractExternalSubgraphBundles(k.stopX,k.V_DFI,k.externalSubgraph,++m_nodeMarker);
	} else {
		extractExternalSubgraph(k.stopX,k.V_DFI,k.stopXStartnodes,k.stopXEndnodes);
	}
	
	#ifdef STATISTICS
	stat << "\nexternalSubgraph Y:";
	#endif
	if (m_bundles) {
		extractExternalSubgraphBundles(k.stopY,k.V_DFI,k.externalSubgraph,++m_nodeMarker);
	} else {
		extractExternalSubgraph(k.stopY,k.V_DFI,k.stopYStartnodes,k.stopYEndnodes);
	}
	
	// pass pertinent nodes in the lists of possible different minor-types
	splitInMinorTypes(k.externalFacePath,++m_nodeMarker);
	
	// extract pertinent subgraphs from all w-nodes to V
	if (m_bundles) {
		extractPertinentSubgraphBundles(k.wNodes,k.V,k.pertinentSubgraph,++m_nodeMarker);
	} else {
		extractPertinentSubgraph(k.wNodes,k.V);
	}
	
	#ifdef STATISTICS
	stat << "\nexternal face path: ";
	SListIterator<adjEntry> itAdj;
	for (itAdj=k.externalFacePath.begin();itAdj.valid();++itAdj)
		stat << m_dfi[(*itAdj)->theNode()] << ",";
	stat << "\nhighestXYPaths: ";
	SListIterator<SListPure<adjEntry> > ite;
	for (ite=k.highestXYPaths.begin();ite.valid();++ite) {
		stat << "\n\t" << &*ite << ": ";
		for (itAdj=(*ite).begin();itAdj.valid();++itAdj)
			stat << m_dfi[(*itAdj)->theNode()] << ",";
	}
	stat << "\nzPaths: ";
	SListIterator<adjEntry> itZ;
	for (ite=k.zPaths.begin();ite.valid();++ite) {
		stat << "\n\t" << &*ite << ": ";
		for (itZ=(*ite).begin();itZ.valid();++itZ)
			stat << m_dfi[(*itZ)->theNode()] << ",";
	}
	stat << "\n\nwNodes:";
	SListIterator<WInfo> itW;
	for (itW=k.wNodes.begin();itW.valid();++itW) {
		WInfo& info(*itW);
		stat << "\n" << m_dfi[info.w] << " (highestPath " << (*itW).highestXYPath;
		stat << ", zPath " << info.zPath << ", pxAbove/pyAbove (";
		stat << info.pxAboveStopX << "/" << info.pyAboveStopY << "), ExternEStart/End/firstAfterW (";
		if (!info.externEStart.valid()) stat << "NULL";
			else stat << m_dfi[(*info.externEStart).theNode];
		stat << "/";
		if (!info.externEEnd.valid()) stat << "NULL";
			else stat << m_dfi[(*info.externEEnd).theNode];
		stat << "/";
		if (!info.firstExternEAfterW) stat << "NULL";
			else stat << m_dfi[info.firstExternEAfterW];
		stat << "), Minors ";
		if (info.minorType & WInfo::A) stat << "A";
		if (info.minorType & WInfo::B) stat << "B";
		if (info.minorType & WInfo::C) stat << "C";
		if (info.minorType & WInfo::D) stat << "D";
		if (info.minorType & WInfo::E) stat << "E";
		stat << ")";
	}
	stat << "\n\nexternE: " << flush;
	SListIterator<ExternE> itExternE;
	SListIterator<int> itInt;
	for (itExternE=k.externE.begin(); itExternE.valid(); ++itExternE) {
		stat << m_dfi[(*itExternE).theNode] << "(";
		for (itInt=(*itExternE).startnodes.begin(); itInt.valid(); ++itInt)
			stat << *itInt << " ";
		stat << "),";
	}
	stat << "\nnodeMarker=" << m_nodeMarker;
	#endif
	
	// add Kuratowski to KuratowskisOnNode
	allKuratowskis.pushBack(k);
	
	// reverse flipping
//	pBM->flipBicomp(-m_dfi[k.R],++m_nodeMarker,m_wasHere,false,false);
	
	// graphical output
	OGDF_ASSERT(m_bundles || k.pertinentSubgraph.empty());
	#ifdef FILE_OUTPUT
	if (!m_bundles) {
		// mark last pertinent edges yellow
		SListIterator<SListPure<edge> > itPaths;
		for (itW = k.wNodes.begin(); itW.valid(); ++itW) {
			for (itPaths = (*itW).pertinentPaths.begin(); itPaths.valid(); ++itPaths)
				k.pertinentSubgraph.pushBack((*itPaths).back());
		}
	}
	Output::kuratowskiStructureOutput(*pBM,m_ga,k.pertinentSubgraph,
										k.externalSubgraph,KURATOWSKI_FILE,k);
	if (!m_bundles) k.pertinentSubgraph.clear();
	#endif
	#ifdef STATISTICS
	stat << "\n------------------------------------------------------------";
	stat << "\n------------------------------------------------------------";
	#endif
}


}
