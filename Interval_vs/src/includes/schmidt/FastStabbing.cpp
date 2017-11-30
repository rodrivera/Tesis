/************************************************************
Copyright (C) 2009 Jens M. Schmidt

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
or 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
************************************************************/

#include "FastStabbing.h"

namespace ogdf {

// verifies correctness of output, 0=correct, 1=not correct
#ifdef OGDF_DEBUG
bool FastStabbing::verify(std::vector<interval*> output, const int& q) {
	interval* temp;
	interval* last = NULL;
	while (!output.empty()) {
		temp = output.back();
		output.pop_back();
		if (last && (*temp < *last)) {
			cerr << "\nerror: interval " << temp << " not in order (not after " << last << ")\n";
			return 1;
		}
		temp->stabbed = true;
		last = temp;
	}
	bool stabs;
	for (int i=0; i<n; ++i) {
		stabs = a[i].l <= q && q <= a[i].r;
		if (a[i].stabbed != stabs) {
			cerr << "\nerror: interval " << i << " (" << &a[i] << ") should be" << (stabs ? " stabbed\n" : " not stabbed\n");
			return 1;
		}
		a[i].stabbed = false;
	}
	return 0;
}
#endif

void FastStabbing::preprocessing() {
	// create smaller lists and event lists
	int i,l,starting=-1;
	for (i=0; i<n; ++i) {
		l = a[i].l;
		if (l != starting) {
			// sorted event lists for sweepline
			eventlist[a[i].r].pushBack(&a[i]);
			eventlist[l].pushBack(&a[i]);
		} else {
			OGDF_ASSERT(a[i-1].l == l && a[i-1].r > a[i].r);
			a[i-1].smaller = &a[i];
		}
		starting = l;
	}
	// sweep line
	ListPure<interval*> L; // status list
	ListIterator<interval*> it;
	interval* temp;
	interval* last;
	for (i=1; i<=bigN; ++i) {

		//cout << "> i: " << i << endl;
		// interval with starting point i
		if (!eventlist[i].empty()) {
			temp = eventlist[i].back();
			if (temp->l == i) {
				temp->pIt = L.pushBack(temp);
				eventlist[i].popBack();
			}
		}

		interval* DELETE_LAST;
		OGDF_ASSERT(!L.empty() || eventlist[i].empty());
		if (!L.empty()) {
			// compute stop[i]
			stop[i] = L.back();
			stop2[i] = (i<3 || stop[i]->l > stop2[i-1]->l)? stop[i] : stop2[i-1];
			// intervals with end points i

			if(stop[i] != DELETE_LAST)
			DELETE_LAST = stop[i];

			//cout << ">> start1["<<i<<"]  : " << stop[i]->l << ", " << stop[i]->r << endl;
			//cout << ">> start2["<<i<<"]  : " << stop2[i]->l << ", " << stop2[i]->r << endl << "---" << endl;
			for (it = eventlist[i].rbegin(); it.valid(); --it) {
				temp = *it;
				if (temp->pIt.pred().valid()) {
					last = *temp->pIt.pred();
					//cout << "> " << last->l << ", " << last->r << " --> " << temp->l << ", " << temp->r << endl; // PARENT -> CHILD
				} else last = &dummy;
				temp->parent = last;
				temp->leftsibling = last->rightchild;
				last->rightchild = temp;
				L.del(temp->pIt);
				last = temp;
			}
		} else {
			if (i > 1){
				stop2[i] = stop2[i-1];
			}
		}
		
		//if (i>1) cout << ">> start_2[i]: " << stop2[i]->l << ", " << stop2[i]->r << endl;

	}
	/*
						cout << " >>> preprocessing:" << endl;
						for (int i = 0; i < stop.size(); ++i){
							cout << i << "("<< stop[i] << ")_";
						}
						cout << " >>> <<< " << endl;
						*/
}

// stabbing query
void FastStabbing::query(const int& ql, const int& qr, std::vector<interval*>& output, const bool onlySearch, long& numComparisons) {

	//cout << "### START QUERY WITH.. (" << ql << ", "<< qr << ")" << endl;

	OGDF_ASSERT(ql >= 1 && ql <= bigN+1);
	OGDF_ASSERT(qr >= ql && qr <= bigN+1);
	output.clear();

	if(! (ql >= 1 && ql < bigN+1 && qr >= ql && qr < bigN+1) ){
		cout << "> q = [ " << ql << " , " << qr << " ]" << endl;
		cout << "> D = [ " << 1 << " , " << bigN << " ]" << endl;
		cout << "Out of bounds query." << endl;
		return;
	}

	interval* i;
	interval* temp;
	ListPure<interval*> process;

	interval* start_1Lq = stop[ql];
	interval* start_2Rq = stop2[qr];
 /*
						cout << " >>> in query:" << endl;
						for (int i = 0; i < stop.size(); ++i){
							cout << stop[i] << "_";
						}
						cout << " >>> <<< " << endl;

						cout << "### sizes:   " << stop.size() << " " << stop2.size() << endl;
						cout << "### indexes: " << ql << " " << qr << endl;
 */
	if (start_1Lq == NULL){
		if (start_2Rq == NULL){
			return; // no stabbed intervals
		}
		temp = start_2Rq;
	} else {
		temp = (start_1Lq->l >= start_2Rq->l)? start_1Lq : start_2Rq;
	}
	if (temp->r < ql){
		return; // no stabbed intervals
	}

	if (onlySearch) {
		output.push_back(temp);
		return;
	}

	for (; temp->parent != NULL; temp = temp->parent) {
		process.pushFront(temp);
	}

	// traverse
	while (!process.empty()) {
		i = process.popBackRet();
		output.push_back(i);
		
		temp = i->smaller;
		while (temp != NULL) {
			++numComparisons;
			if (ql > temp->r) break;
			output.push_back(temp);
			#ifdef OGDF_DEBUG
			#endif
			temp = temp->smaller;
		}

		// go along rightmost path of pa
		temp = i->leftsibling;
		while (temp) {
			++numComparisons;
			if (temp->r < ql) break;
			process.pushBack(temp);
			temp = temp->rightchild;
		}
	}
	OGDF_ASSERT(verify(output,ql) == 0);
	//cout << "### END QUERY WITH.. " << output.size() << " results." << endl;
}


	size_t FastStabbing::size() {
		size_t totalSize = 	sizeof(*this) + 
												(sizeof(a[0])-2*sizeof(long))*a.size() + 
												sizeof(stop[0])*stop.size() + 
												sizeof(stop2[0])*stop2.size() + 
												sizeof(eventlist[0])*eventlist.size();

		return totalSize;
	}

	int FastStabbing::height() {
		interval* curr = &dummy;
		return heightAUX(curr) - 1;
	}
	int FastStabbing::heightAUX(interval* curr) {
		if(!curr) return 0;
		//cout << "*" << max( heightAUX(curr->leftsibling) , heightAUX(curr->rightchild) ) << endl;
		return max( heightAUX(curr->leftsibling) , 1 + heightAUX(curr->rightchild) );
	}

	void FastStabbing::print() {
		interval* curr = &dummy;
		cout << "(R)->";
		printAUX(curr->rightchild,1);
	}
	void FastStabbing::printAUX(interval* curr, int level) {
		if(!curr){ 
			cout << "(L)";
			return;
		}
		cout << "("<<level<<")";
		//cout << "*" << max( heightAUX(curr->leftsibling) , heightAUX(curr->rightchild) ) << endl;
		cout << "->";
		printAUX(curr->rightchild,level+1);
		cout << endl;
		for (int i = 0; i < level; ++i) cout << "     ";
		cout << " |" << endl;
		for (int i = 0; i < level; ++i) cout << "     ";
		printAUX(curr->leftsibling,level);
	}

}