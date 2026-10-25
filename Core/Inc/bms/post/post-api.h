/*!
 * \file            post-api.h
 * \authors         Thomas Moretti [majorfrost173@gmail.com]
 * \date            2026-08-08
 *
 * \brief           Module dedicated to running a power-on self-test (POST).
 */

#ifndef POST_API_H
#define POST_API_H

#include "post.h"

/*!
 * \brief            Run the power-on self-test.
 *
 * \retval           POST_RC_OK on success.
 * \retval           POST_RC_UNINITIALIZED_MODULE if a module failed to initialize.
 * \retval           POST_RC_ERROR if POST has been unsuccessful.
 */
enum PostReturnCode post_api_run(struct PostInitData *post_init_data);

#endif // POST_API_H
