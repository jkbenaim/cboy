/*************************************************************************
 *   Cboy, a Game Boy emulator
 *   Copyright (C) 2013 jkbenaim
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ************************************************************************/

// int pqueue_init( pqueue_root *root );
// int pqueue_isEmpty( pqueue_root *root );
// int pqueue_peek( pqueue_root *root );
// int pqueue_pop( pqueue_root *root );
// int pqueue_hasNext( pqueue_root *root );
// int pqueue_pass( pqueue_root *root, int time );
// int pqueue_insert( pqueue_root *root, int time, int message );

#include <stdlib.h>
#include "pqueue.h"

int pqueue_init(pqueue_root * root)
{
	pqueue_root *newRoot = calloc(1, sizeof(pqueue_root));

	if (newRoot) {
		root = newRoot;
		return 0;
	} else {
		return 1;
	}
}

int pqueue_isEmpty(pqueue_root * root)
{
	return root == NULL || root->first == NULL;
}

int pqueue_peek(pqueue_root * root)
{
	if (pqueue_isEmpty(root))
		return 0;
	else
		return root->first->message;
}

int pqueue_pop(pqueue_root * root)
{
	// store the value from peek for later
	int returnValue = pqueue_peek(root);

	if (root == NULL) {
		// nothing to do
	} else {
		// root is non-null. do we have an element to pop?
		if (pqueue_isEmpty(root)) {
			// root is non-null but the queue is empty. nothing to do.
		} else {
			// root is non-null and not empty.
			// remember the priority of the element before popping.
			int priorityOfPoppedElement = root->first->priority;

			// pop the first element.
			root->first = root->first->next;

			// adjust the priority of the new first element.
			root->first->priority += priorityOfPoppedElement;
		}
	}
	return returnValue;
}

int pqueue_hasNext(pqueue_root * root)
{
	// this is significantly different from isEmpty:
	// hasNext tells you if the queue has an element *and*
	// if the next element is ready to be popped (that is, if
	// its priority is <= 0)

	if (pqueue_isEmpty(root)) {
		// the queue is empty, so we can't have a "next"
		return 1;
	} else {
		// the queue is not empty. check the priority of the first element.
		int priority = root->first->priority;
		if (priority <= 0) {
			// this element is due (or past due), so return true.
			return 0;
		} else {
			// priority is >0
			// this element is not due, so return false.
			return 1;
		}
	}
}

void pqueue_pass(pqueue_root * root, int time)
{
	if (pqueue_isEmpty(root)) {
		// the queue is empty. nothing to do.
	} else {
		// pass time on the first element.
		root->first->priority -= time;
	}
}

int pqueue_insert(pqueue_root * root, int priority, int message)
{
	if (root == NULL)
		return 1;

	if (priority < 0)
		return 2;

	int didInsertionHappen = 1;	// false

	// How to insert into the priority queue:
	// Starting from the root, walk the list of elements.
	// Find the element with the lowest priority that is greater
	// than or equal to the new element's priority. Mark this
	// element. It is possible that no suitable element will be found.
	// 
	// If no suitable element is found:
	//   The new element will occur after all other elements in the queue.
	//   Append the new element to the end of the queue.
	// Otherwise,
	//   A suitable element was found and marked.
	//   The new element should occur just before the element that was marked.
	//   Rather than placing the new element before the mark, we will copy and
	//   modify the mark.
	//   Make a copy of the mark. Insert the copy after the mark. Then
	//   modify the mark to be the new element.
	//
	// Oh: also, the queue could be totally empty. In that case, just make a new element.

	if (pqueue_isEmpty(root)) {
		pqueue_node *newElement = calloc(1, sizeof(pqueue_node));
		if (newElement) {
			newElement->priority = priority;
			newElement->message = message;
			root->first = newElement;
			didInsertionHappen = 0;	// true
		} else {
			// calloc failed
			return 3;
		}
	} else {
		// The queue is not empty.
		// We're doing two things at once, here:
		//   -hunting for a mark (described above), and
		//   -checking if the new element should go all the way at the end of the queue.
		pqueue_node *candidate = root->first;
		int prioritySoFar = 0;
		while (candidate != NULL) {
			// Is this the mark?
			if (candidate->priority >= priority) {
				// We've found our mark!

				// First, calculate the priorities of both elements.
				int priorityOfInsertedElement =
				    priority - prioritySoFar;
				int priorityOfCopiedMark =
				    candidate->priority -
				    priorityOfInsertedElement;

				// Copy the mark to a new element and insert it after the mark.
				pqueue_node *newElement =
				    calloc(1, sizeof(pqueue_node));
				newElement->priority = priorityOfCopiedMark;
				newElement->message = candidate->message;
				newElement->next = candidate->next;
				candidate->next = newElement;

				// Cannibalize the mark to become the element we wanted to insert.
				candidate->priority = priorityOfInsertedElement;
				candidate->message = message;
				didInsertionHappen = 0;	// true
				break;
			}
			// OK, this isn't our mark.
			// Is this the last element in the queue?
			if (candidate->next == NULL) {
				// This is the last element in the queue, so
				// insert at the end and break.
				pqueue_node *newElement =
				    calloc(1, sizeof(pqueue_node));
				newElement->priority = priority - prioritySoFar;
				newElement->message = message;
				candidate->next = newElement;
				didInsertionHappen = 0;	// true
				break;
			}
			// We didn't find our mark OR the end of the queue.
			// Go to the next element and try again.
			prioritySoFar += candidate->priority;
			candidate = candidate->next;
		}
	}			// queue not empty

	if (didInsertionHappen == 0)
		return 0;
	else
		return 4;
}
