#|
    Copyright (C) 2012 Ivan Radicek

    This file is part of Bayes.

    Bayes is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Bayes is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Bayes.  If not, see <http://www.gnu.org/licenses/>.
|#

;;; Utility functions ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defun duplicates-p (list)
  "Checks if there are duplicates in list"
  (/= (length list)
      (length (remove-duplicates list :test #'equal))))

(defun append-items (list &rest items)
  "Appends items to end of a list"
  (append list items))

(defun eql-f (f1 f2 &optional (err 0.0001))
  "Compares to float numbers to be very similar"
  (<= (abs (- f1 f2)) err))

(defun assign-to-index (dims vals)
  "Convert nodes assigments to index in table"
  (let ((i 0)
	(tmp 1))
    (loop
       for val in (reverse vals)
       for dim in (reverse dims)
	 do (incf i (* val tmp))
	 do (setf tmp (* tmp dim)))
    i))

(defun assoc-replace (alist item newval)
  "Replaces item in assoc, creating new list"
  (append-items (remove item alist :key #'car :test #'equal) (list item newval)))

;;; Class definitions ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; Class representing Bayes network
(defclass network nil
  ((name :initarg :name
	 :initform (error "Network name not specified")
	 :accessor name)
  (nodes :initform nil)))

;;; Class representing node in Bayes network
(defclass node nil
  ((vals :initarg :vals
	 :initform '("T" "F")
	 :reader vals)
   (parents :initarg :parents
	    :initform nil
	    :reader parents)
   (children :initform nil
	     :reader children)
   (table :initarg :table
	  :initform (error "Table was not specified")
	  :reader table)
v   (meta :initarg :meta
	 :initform nil
	 :reader meta)))


;;; Node and network  manipulation methods ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defgeneric get-node (net name) (:documentation  "Gets node from network by name"))
(defmethod get-node ((net network) name)
  (cdr (assoc name (slot-value net 'nodes) :test #'equal)))

(defgeneric get-nodes (net &rest names) (:documentation "Gets list of nodes from network by names"))
(defmethod get-nodes ((net network) &rest names)
  (remove-duplicates 
   (mapcar #'(lambda (name) (get-node net name)) names)
   :test #'equal))

(defgeneric get-node-names (net) (:documentation "Gets node names from network"))
(defmethod get-node-names ((net network))
  (mapcar #'car (slot-value net 'nodes)))

(defgeneric add-node-children (net name child-name) (:documentation  "Adds children to node"))
(defmethod add-node-children ((net network) name child-name)
  (let ((node (get-node net name)))
    (unless node (error (format nil "Node ~A does not exist" name)))
    (unless (get-node net child-name)
      (error (format nil "Child node ~A does not exist" child-name)))
    (with-slots (children) node 
      (setf children (append-items children child-name)))))

(defgeneric check-node-table (net n) (:documentation "Checks if node table is valid"))
(defmethod check-node-table ((net network) node-name)
  (let* ((n (get-node net node-name))
	 (tab (table n))
	 (step (length (vals n)))
	 (total (/ (length tab) step))
	 (parent-vals (mapcar #'(lambda (x) (length (vals (get-node net x))))
			     (parents n))))
    (unless (= (* step
		  (reduce #'* parent-vals))
	       (length tab))
      (error (format nil "Invalid table size for node ~A" node-name)))
    (loop for i to (- total 1)
	 do (let* ((i1 (* i step))
		  (i2 (- (+ i1 step) 1))
		   (s (loop for j from i1 to i2
			 summing (nth j tab))))
	      (unless (eql-f 1.0 s)
		(error (format nil "Invalid table sum for rows from ~D to ~D  (~F) in node ~A"
			       (+ 1 i1) (+ 1 i2) s node-name)))))))

(defgeneric add-node (net node name) (:documentation "Adds node to network"))
(defmethod add-node ((net network) (n node) name)
  (when (get-node net name)
    (error (format nil "Duplicate node name: ~A" name)))
  (when (duplicates-p (vals n))
    (error (format nil "Duplicate values in node ~A" name)))
  (when (> 2 (length (vals n)))
    (error (format nil "Less than two values in node ~A" name)))
  (loop for parent in (slot-value n 'parents)
     do (unless (member parent (get-node-names net) :test #'equal)
	  (error (format nil "Parent node ~A does not exist" parent))))
  (with-slots (nodes) net
    (setf nodes (append-items nodes (cons name n))))
  (loop for parent in (slot-value n 'parents)
       do (add-node-children net parent name))
  (check-node-table net name))

(defgeneric probability (net name &rest node-vals) (:documentation "Gets probability for values"))
(defmethod probability ((net network) name &rest node-vals)
  (let* ((node (get-node net name))
	 (dims
	  (append-items 
	   (mapcar #'(lambda (x) (length (vals (get-node net x))))
		   (parents node))
	   (length (vals node))))
	 (vals nil))
    (dolist (parent (parents node))
      (let* ((parent-node (get-node net parent))
	     (par-val (second (assoc parent node-vals :test #'equal)))
	     (par-pos (position par-val (vals parent-node) :test #'equal)))
	(setf vals (append-items vals par-pos))))
    (let* ((self-val (second (assoc name node-vals :test #'equal)))
	   (self-pos (position self-val (vals node) :test #'equal)))
      (setf vals (append-items vals self-pos)))
    (nth (assign-to-index dims vals) (table node))))


;;; Inference algorithms ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defparameter *inference-algorithms* nil "Global list of all available algorithms")

(defun evidence-value (name evidence)
  "Extracts value from eviddence for given name"
  (second (assoc name evidence :test #'equal)))

(defun normalize-node-values (vals)
  "Normalize one node values list"
  (let ((sum (reduce #'+ (mapcar #'second vals)))
	(n (length vals)))
    (loop for val in vals
       collecting (list (first val)
			(if (> sum 0)
			    (* 1.0 (/ (second val) sum))
			    (/ 1.0 n))))))

(defun normalize-values (nodes)
  "Normalizes list of nodes and values so each node sum up to 1.0"
  (loop for node in nodes
     collecting (list (first node)
		      (normalize-node-values (second node)))))

(defgeneric enumeration-node (net name &rest evidence)
  (:documentation "Calculates probabilities of one node using enumberation 
algorithm; full enumeration algorithm uses this method on every node"))
(defmethod enumeration-node ((net network) name &rest evidence)
  (let* ((node (get-node net name))
	 (node-names (get-node-names net))
	 (evidence-names (mapcar #'car evidence))
	 (non-evidence-names
	  (remove-if #'(lambda (x)
			 (member x evidence-names :test #'equal)) node-names))
	 (loop-names (remove name non-evidence-names :test #'equal))
	 (const (not (null (member name evidence-names :test #'equal))))
	 (results nil))
    ;; Recursive function for enumeration
    (labels
	((calculate-p (loops evidence)
	   (cond ((> (length loops) 0) ; Loop more variables
		  (loop for val in (vals (get-node net (first loops)))
		     summing (calculate-p (rest loops)
					  (cons (list (first loops) val)
						evidence))))
		 ;; Multiply all probabilities
		 (t (apply #'* (loop for n in node-names
				  collecting (apply #'probability 
						    (append (list net n)
							    evidence))))))))
      ;; Put each value into evidence and calculate probability
      (dolist (val (vals node))
	(let ((p (cond (const (if (equal val (evidence-value name evidence))
				  1.0 0.0))
		       (t (calculate-p loop-names
				       (cons (list name val) evidence))))))
	  (push (list val p) results))))
    (reverse results)))		       

(defgeneric enumeration (net &rest evidence)
  (:documentation "Calculate all probabilites in network using enumeration
algorithm"))
(defmethod enumeration ((net network) &rest evidence)
  (let ((start-time (get-internal-run-time)))
    (values
     (normalize-values 
      (loop for node-name in (get-node-names net)
	 collecting
	   (list node-name
		 (apply #'enumeration-node
			(append (list net node-name) evidence)))))
     (* 1.0 (/ (- (get-internal-run-time) start-time)
	       internal-time-units-per-second)) nil)))

;;; Add new algorithm to list of algorithms
(push `("Enumeration" ,#'enumeration nil) *inference-algorithms*)

(defgeneric make-value-counter (net &rest evidence)
  (:documentation "Creates table for counting values in sampling algorithms"))
(defmethod make-value-counter ((net network) &rest evidence-names)
  (loop for node in 
       (remove-if #'(lambda (x) (member x evidence-names :test #'equal))
		  (get-node-names net))
     collecting
       (list node (loop for val in (vals (get-node net node))
		     collecting (list val 0)))))

(defun update-value-counter (counter node value &optional (amt 1))
  "Updates table value counter table"
  (incf (second
	 (assoc value (second (assoc node counter :test #'equal))
		:test #'equal)) amt))

(defun prob-diff (p1 p2)
  "Compares total difference between to probability distributionsUpdates table
value counter table"
  (loop
     for node1 in p1
     for node2 in p2
     maximizing (loop
		for v1 in (second node1)
		for v2 in (second node2)
		maximizing (abs (- (second v1) (second v2))))))

(defun wheel-select (items)
  "Selects item by propotional probability - roulete wheel"
  (let ((rand-val (random 1.0))
	(temp-p 0.0))
    (dolist (item items)
      (let ((value (first item))
	    (prob (second item)))
	(incf temp-p prob)
	(when (>= temp-p rand-val)
	  (return value))))))
     
(defgeneric sample-node (net node-name parent-vals)
  (:documentation "Samples node based on local probabilities and parent value"))
(defmethod sample-node ((net network) node-name parent-vals)
  (let ((node (get-node net node-name)))
    (wheel-select
     (loop for val in (vals node)
	collecting
	  (let* ((all-vals (append-items parent-vals (list node-name val)))
		 (prob (apply #'probability 
			      (append (list net node-name) all-vals))))
	    (list val prob))))))
  
(defgeneric sample-node-mb (net node-name mb-vals)
  (:documentation "Samples node based on local probabilities and
Markov Blanket values"))
(defmethod sample-node-mb ((net network) node-name mb-vals)
  (let* ((node (get-node net node-name))
	 (children (children node)))
    (wheel-select
     (normalize-node-values
      (loop for val in (vals node)
	 collecting
	   (let* ((vals (assoc-replace mb-vals node-name val))
		  (prob (apply #'probability (append (list net node-name)
						     vals))))
	     (dolist (child-name children)
	       (setf prob (* prob (apply #'probability
					 (append (list net child-name)
						 vals)))))
	     (list val prob)))))))

(defun random-element (list)
  "Selects random item from list"
  (nth (random (length list)) list))

(defparameter *diff-small-value* 0.0005 "Sampling global parameters")
(defparameter *diff-check-period* (round (/ *diff-small-value*))
  "Sampling global parameters")

(defgeneric gibbs-sampling (net n &rest evidence)
  (:documentation "Calculate all probabilities in network using
gibbs sampling algorithm"))
(defmethod gibbs-sampling ((net network) n &rest evidence)
  (setf *random-state* (make-random-state t))
  (let* ((start-time (get-internal-run-time))
	 (node-names (get-node-names net))
	 (evidence-names (mapcar #'car evidence))
	 (nonevidence-names (set-difference node-names evidence-names
					    :test #'equal))
	 (value-counter (make-value-counter net))
	 (current-vals evidence)
	 (done 0))
    (dolist (node-name nonevidence-names)
      (setf current-vals
	    (append-items current-vals
			  (list node-name (random-element
					   (vals (get-node net node-name)))))))
    (do* ((old-probs nil)
	  (probs nil)
	  (new-vals nil nil)
	  (sample))
	 ((when (> n 0) (>= done n)))
      (incf done)
      (dolist (node-name node-names)
	(let ((evidence-p (member node-name evidence-names :test #'equal)))
	  (setf sample
		(if evidence-p
		    (second (assoc node-name evidence :test #'equal))
		    (sample-node-mb net node-name current-vals)))
	  (push (list node-name sample) new-vals)
	  ;(setf current-vals (assoc-replace current-vals node-name sample))
	  ;(update-value-counter value-counter node-name sample)
	  ))
      (setf current-vals new-vals)
      (dolist (node-name node-names)
	(update-value-counter
	 value-counter node-name (second (assoc node-name
						current-vals :test #'equal))))
      (when (zerop (mod done *diff-check-period*))
	(setf old-probs probs)
	(setf probs (normalize-values value-counter))
	(when (and old-probs
		   (<= (prob-diff probs old-probs) *diff-small-value*))
	  (return))))
    (values (normalize-values value-counter)
	    (* 1.0 (/ (- (get-internal-run-time) start-time)
		      internal-time-units-per-second)) done)))	        

;; Add new algorithm to list of algorithms
(push `("Gibbs sampling" ,#'gibbs-sampling t) *inference-algorithms*)

(defgeneric rejection-sampling (net n &rest evidence)
  (:documentation "Calculate all probabilities in network using
rejection samppling algorithm"))
(defmethod rejection-sampling ((net network) n &rest evidence)
  (setf *random-state* (make-random-state t))
  (let* ((start-time (get-internal-run-time))
	 (evidence-names (mapcar #'car evidence))
	 (node-names (get-node-names net))
	 (done 0)
	 (value-counter (make-value-counter net)))
    (do* ((old-probs nil)
	  (current-vals nil nil)
	  (probs nil))
	 ((when (> n 0) (>= done n)))
      (block iteration
	(incf done)
	(dolist (node node-names)
	  (let ((sample (sample-node net node current-vals)))
	    (if (or (not (member node evidence-names :test #'equal))
		    (equal sample (second (assoc node evidence :test #'equal))))
		(setf current-vals 
		      (append-items current-vals (list node sample)))
		(return-from iteration))))
	(dolist (val current-vals)
	  (update-value-counter value-counter (first val) (second val))))
      (when (zerop (mod done *diff-check-period*))
	(setf old-probs probs)
	(setf probs (normalize-values value-counter))
	(when (and old-probs 
		   (<= (prob-diff probs old-probs) *diff-small-value*))
	  (return))))
    (values (normalize-values value-counter)
	    (* 1.0 (/ (- (get-internal-run-time) start-time)
	       internal-time-units-per-second)) done)))

;;; Add new algorithm to list of algorithms
(push `("Rejection sampling" ,#'rejection-sampling t) *inference-algorithms*)

(defgeneric likelihood-weighting (net n &rest evidence)
  (:documentation "Calculate all probabilities in network using
likelihood weighting algorithm"))
(defmethod likelihood-weighting ((net network) n &rest evidence)
  (setf *random-state* (make-random-state t))
  (let ((start-time (get-internal-run-time))
	(evidence-names (mapcar #'car evidence))
	(node-names (get-node-names net))
	(value-counter (make-value-counter net))
	(done 0))
    (do* ((old-probs nil)
	  (current-vals nil nil)
	  (w 1.0 1.0)
	  (probs nil)
	  (sample))
	 ((when (> n 0) (>= done n)))
      (incf done)
      (dolist (node node-names)
	(let ((evidence-p (member node evidence-names :test #'equal)))
	  (setf sample
		(if evidence-p
		    (second (assoc node evidence :test #'equal))
		    (sample-node net node current-vals)))
	  (setf current-vals (append-items current-vals (list node sample)))
	  (when evidence-p 
	    (setf w (* w (apply #'probability
				(append (list net node) current-vals)))))))
      (dolist (val current-vals)
	(update-value-counter value-counter (first val) (second val) w))
      (when (zerop (mod done *diff-check-period*))
	(setf old-probs probs)
	(setf probs (normalize-values value-counter))
	(when (and old-probs
		   (<= (prob-diff probs old-probs) *diff-small-value*))
	  (return))))
    (values (normalize-values value-counter)
	    (* 1.0 (/ (- (get-internal-run-time) start-time)
		      internal-time-units-per-second)) done)))


;;; Add new algorithm to list of algorithms
(push `("Likelihood weighting" ,#'likelihood-weighting t) *inference-algorithms*)

;;; Reading and printing of Bayes networks ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defun make-bayes-network (name &rest nodes)
  "Creates Bayes network with nodes; nodes should be p-lists containing
properties:  - name, vals, parents, table, meta"
  (let ((network (make-instance 'network :name name))
	(created-nodes nil)
	(node-num (length nodes)))
    (when (= node-num 0) (error "Empty network"))
    (do () ((= node-num (length created-nodes)))
      (let ((done 0))
	(dolist (node nodes)
	  (block node-iterate
	    ;; Node is not created, but all his parents are:
	    ;(when (getf node :done) (return))
	    (dolist (parent (getf node :parents))
	      (unless (member parent created-nodes :test #'equal)
		(return-from node-iterate)))
	    ;; Create node and increase counter
	    (let* ((name (getf node :name))
		   (vals (getf node :vals))
		   (parents (getf node :parents))
		   (table (getf node :table))
		   (meta (getf node :meta))
		   (n (make-instance 'node :vals vals :parents parents
				     :table table :meta meta)))
	      (incf done)
	      (setf nodes (remove node nodes :test #'equal))
	      (push name created-nodes)
	      (add-node network n name))))
	(when (= done 0)
	  (error "Cycle found in network"))))
    network))

(defun read-bayes-network (&rest cmds)
  "Reads Bayes network"
  (let ((network-name nil)
	(node-list nil))
    (loop for cmd in cmds
	 do (let ((name (first cmd))
		  (options (rest cmd)))
	      (cond ((eql name 'network)
		     (setf network-name (getf options :name)))
		    ((eql name 'node) (push options node-list))
		    (t (error (format nil "Unknown command ~A"
				      (symbol-name name)))))))
    (apply #'make-bayes-network (cons network-name node-list))))

(defun print-bayes-network (network)
  "Exports Bayes network in the same format it can read it"
  (let ((network-list nil))
    (push (list 'network :name (name network)) network-list)
    (dolist (node-item (slot-value network 'nodes))
      (let ((node (cdr node-item))
	    (name (car node-item)))
	(push (list 'node :name name :vals (vals node)
		    :parents (parents node) :table (table node)
		    :meta (meta node)) network-list)))
    (reverse network-list)))
	  