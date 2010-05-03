/*
 * lppodtransformations.cc
 *
 *  Created on: 23/gen/2010
 *      Author: confalonieri
 */


void transformLppod(LppodProgram *p) {

	/*
	 * P \rightarrow_{PC} P', if $P$ contains:
	 *  	- a rule $r = \alpha: {\cal C}^{\times} \leftarrow {\cal B}^+, \; not \; {\cal B}^-$ which has an atom $b$ such that,
	 *  	- $b \in {\cal B}^+$ and $b \in {\cal B}^-$,
	 *  	-> $P' = P \backslash \{r\}$
	*/

	/*
	 * P \rightarrow_{PRED^+} P', if
	 * 		- there is a rule $r = \alpha: {\cal C}^{\times} \leftarrow {\cal B}^+, \; not \; ({\cal B}^- \cup \{b\} )$ in $P$ and,
	 * 		- such that $b \notin HEAD(P)$, and
	 * 		-> $P' = (P \backslash  \{r\}) \cup \{\alpha: {\cal C}^{\times} \leftarrow {\cal B}^+, \; not \; {\cal B}^- \}$
	 */

	/*
	 * P \rightarrow_{PRED^-} P'$, if $P$ contains the rules:
	 * 		- $r = \alpha: a \leftarrow \top $, and
	 * 		- $r' = \beta: {\cal C}^{\times} \leftarrow {\cal B}^+, \; not \; ({\cal B}^- \cup \{a\} )$,
	 * 		-> $P' = (P \backslash  \{r'\}) $
	 */

	/*
	 * P \rightarrow_{PS} P', if $P$ contains:
	 * 		- a fact $ \alpha: a \leftarrow \top$ and
	 * 		- a rule $r = \beta: {\cal C}^{\times} \leftarrow {\cal B}^+, \; not \; {\cal B}^-$ such that $a \in {\cal B}^+$,
	 * 		-> $P' = (P \backslash  \{r\}) \cup \{GLB\{\alpha,\beta\}: {\cal C}^{\times} \leftarrow ({\cal B}^+\backslash\{a\}), \; not \; {\cal B}^- \}$
	 */


	/*
	 * P \rightarrow_{PF} P', if $P$ contains:
	 * 		- a rule $r = \alpha: {\cal C}^{\times} \leftarrow {\cal B}^+, \; not \; {\cal B}^-$ such that
	 * 		- $a \in {\cal B}^+$ and $a \notin HEAD(P)$, and
	 * 		-> P' = (P \backslash  \{r\})$.
	 *
	 */
}
