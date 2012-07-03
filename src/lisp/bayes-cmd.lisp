;;;
;;; Command line utility for Bayes network
;;;

(load "bayes.lisp")

;;; Current network
(defparameter *network* nil)

;;; Outputs a command
(defun output (cmd &rest options)
  (do () ((not (and (listp options) (= (length options) 1)
		    (listp (first options)))))
    (setf options (first options)))
  (format t "~&~S~%" (append (list cmd) options)))

;;; Outputs "friendly" error
(defun output-error (err)
    ;(output "ERROR" (simple-condition-format-control err)))
  (output "ERROR" err))

;;; Loads network on which inference is done
(defun load-network (options)
  (setf *network* (apply #'read-bayes-network options))
  (output "INFO" "Network loaded."))

;;; Loads network from file
(defun load-network-from-file (file-name &optional output-cmds)
  (with-open-file (file file-name :if-does-not-exist :error)
    (funcall #'load-network (read file))
    (when output-cmds
      (output "NETWORK-NAME" (slot-value *network* 'name))
      (dolist (node-l (slot-value *network* 'nodes))
	(let* ((name (car node-l))
	       (node (cdr node-l)))
	  (output "NODE-NAME" name)
	  (output "NODE-VALS" (cons name (slot-value node 'vals)))
	  (let ((metaval (meta node)))
	    (when metaval (loop for (key value) on metaval by #'cddr
			     do (output "NODE-META" name key value))))))
      (dolist (node-l (slot-value *network* 'nodes))
	(let* ((name (car node-l))
	       (node (cdr node-l)))
	  (dolist (parent (slot-value node 'parents))
	    (output "NODE-PARENT" name parent))
	  (output "NODE-TABLE" (cons name (slot-value node 'table))))))
    (output "INFO" (format nil "Network ~S loaded." file-name))
    (output "LOAD-FILE-DONE")))

;;; Saves network to a file
(defun save-file (file-name)
  (with-open-file (file file-name :direction :output :if-does-not-exist :create
			:if-exists :supersede)
    (print (print-bayes-network *network*) file))
  (output "INFO" (format nil "Network ~S saved." file-name))
  (output "FILE-SAVE-DONE"))

;;; Does inference on network
(defun query (options)
  (let* ((algorithm-name (first options))
	 (algorithm (assoc algorithm-name *inference-algorithms* :test #'equal))
	 (algorithm-method (second algorithm))
	 (param-required (third algorithm))
	 (param (when param-required (second options)))
	 (evidence (if param-required (cddr options) (cdr options)))
	 (all-params (append (if param-required (list *network* param)
				 (list *network*))
			     evidence)))
    (multiple-value-bind (result time iterations)
	(apply algorithm-method all-params)
      (dolist (node result)
	(dolist (val (second node))
	  (output "SETVAL" (first node) (first val) (second val))))
      (output "QUERY-DONE")
      (output "INFO"
	      (if iterations
		  (format nil "Query done in ~Fs (~D iterations)."
			  time iterations)
		  (format nil "Query done in ~Fs." time))))))
	  

(defun list-algorithms ()
  (dolist (a *inference-algorithms*)
    (output "ADD-ALGORITHM" (first a) (third a))))

(defun set-option (name value)
  (cond ((equal name "diff-small-value")
	 (setf *diff-small-value* value))
	((equal name "diff-check-period")
	 (setf *diff-check-period* value))
	(t (output-error (format nil "Unknown option ~A" name)))))

(defun main ()
  ;; Greet
  (output "INFO" "Bayes engine v0.1 up and running!")

  ;; Main loop
  (block main-loop
    (loop
       (handler-case
	   (progn
	     (clear-output)
	     (let* ((input (read))
		    (cmd (first input))
		    (options (rest input)))
	       (cond ((eql cmd 'quit) (return-from main-loop))
		     ((eql cmd 'load-network) (load-network options))
		     ((eql cmd 'load-file) (apply #'load-network-from-file
						  options))
		     ((eql cmd 'save-file) (save-file (first options)))
		     ((eql cmd 'query) (query options))
		     ((eql cmd 'algorithms) (list-algorithms))
		     ((eql cmd 'set-option) (set-option (first options)
							(second options)))
		     (t (error (format nil "Unknown command: ~A" cmd))))))
	 (simple-condition (condt)
	   (output-error
	    (apply #'format nil (append 
				 (list (simple-condition-format-control condt))
				 (simple-condition-format-arguments condt)))))
	 (error (condition) (output-error "Unexpected error"))))))
  
;;; Make sure everything is on display
(clear-output)

;;; Output executable
#+linux (sb-ext:save-lisp-and-die "bayes-cmd" :toplevel #'main :executable t)
#+win32 (sb-ext:save-lisp-and-die "bayes-cmd.exe" :toplevel #'main :executable t)