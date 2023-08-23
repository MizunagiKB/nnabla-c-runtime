// Copyright 2018,2019,2020,2021 Sony Corporation.
// Copyright 2021 Sony Group Corporation.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <nnablart/network.h>
#include <nnablart/runtime.h>

#include "context.h"

#include <assert.h>
#include <string.h>

/// @defgroup Examples Examples
/// @{

/// @file
/// @brief Simple callback example.

/// @brief NNB format binary data from Affine_000.nnp
///
/// By default just call @ref rt_add_callback to use call back function.
///
/// But here is some modification to use callback more flexible.
///
/// To generate the 'Affine_000.nnp' file, you can use the following Python
/// code:
/// @code{.py}
/// import nnabla as nn
/// import nnabla.parametric_functions as PF
/// from nnabla.utils.save import save
///
/// x = nn.Variable((1, 4, 6), need_grad=True)
/// y0 = PF.affine(inp=x, n_outmaps=(2, 3), name='network1', base_axis=1)
/// contents = {
///     'networks': [
///         {'name': 'network1',
///          'batch_size': 1,
///          'outputs': {'y0': y0},
///          'names': {'x': x}}],
///     'executors': [
///         {'name': 'inference',
///          'network': 'network1',
///          'data': ['x'],
///          'output': ['y0']}]}

/// save("Affine_000.nnp", contents=contents)
/// @endcode
/// Extracts the network.nntxt from it by unzipping 'Affine_000.nnp'.
/// @code{.sh}
/// $ unzip Affine_000.nnp
/// @endcode
/// Then you should have 'network.nntxt'.
///
/// To use callback, you must set 'function implement flag' in NNB file.
/// You can get template setting file with following command.
/// @code{.sh}
/// $ nnabla_cli nnb_template network.nntxt Affine_000.yaml
/// @endcode
///
/// Then you should be got following 'Affine_000.yaml'.
/// @code{.yaml}
/// functions:
///   '@network1/Affine':
///     implement: 0
/// variables:
///   x: FLOAT32
///   y0: FLOAT32
///   network1/affine/W: FLOAT32
///   network1/affine/b: FLOAT32
/// @endcode
///
/// Edit 'Affine_000.yaml' and save as 'settings.yaml'
/// @code{.yaml}
/// functions:
///   '@network1/Affine':
///     implement: 1
/// @endcode
///
/// NOTE: If you DON'T want to use callback, set implements to 100.
///
/// And convert nntxt to nnb again with settings.yaml.
/// @code{.sh}
/// $ nnabla_cli convert -s settings.yaml
/// nnabla_cli convert -s settings.yaml network.nntxt Affine_000.nnb
/// @endcode
///
/// Then we create binary data with following command.
/// @code{.sh}
/// $ xxd -i <Affine_000.nnb >Affine_000.c
/// @endcode
///
static unsigned char affine_nnb[] = {
    0x02, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x12, 0x00, 0x00, 0x00, 0x20, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
    0x1c, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00,
    0x50, 0x00, 0x00, 0x00, 0x5c, 0x00, 0x00, 0x00, 0x9c, 0x02, 0x00, 0x00,
    0xb0, 0x02, 0x00, 0x00, 0xb8, 0x02, 0x00, 0x00, 0xd0, 0x02, 0x00, 0x00,
    0xe4, 0x02, 0x00, 0x00, 0xf4, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x04, 0x03, 0x00, 0x00, 0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0x18, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
    0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0b, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
    0x09, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x10, 0x00, 0x00, 0x00};

/// @brief dummy input data.
static unsigned char input[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
                                0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                                0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
                                0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
                                0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
                                0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
                                0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40,
                                0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
                                0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,
                                0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
                                0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60};

/// @brief Example callback for execute function.
/// It does nothing but just print function name itself.
static rt_function_error_t cb_exec(rt_function_t *f) {
  WHOAMI("%s", __func__);
  return RT_FUNCTION_ERROR_NOERROR;
}

/// @brief Example callback for free function local context.
/// It does nothing but just print function name itself.
static rt_function_error_t cb_free(rt_function_t *f) {
  WHOAMI("%s", __func__);
  return RT_FUNCTION_ERROR_NOERROR;
}

/// @brief Example callback for allocate function local context.
/// It just does followings.
/// - check func->impl == 1
/// - register @ref cb_exec as function executor.
/// - register @ref cb_free as function local context de-allocator.
///
/// See also @ref rt_initialize_context
static rt_return_value_t cb_alloc(nn_network_t *net, void *function_context) {
  WHOAMI("%s", __func__);
  rt_function_context_t *func = (rt_function_context_t *)function_context;
  if ((int)func->info->impl != 1) {
    return RT_RET_FUNCTION_DONT_MATCH;
  }

  func->func.exec_func = cb_exec;
  func->func.free_local_context_func = cb_free;
  return RT_RET_FUNCTION_MATCH;
}

/// @brief Simple example to use user defined functions.
int main(int argc, char *argv[]) {
  WHOAMI("Callback test.");
  rt_context_pointer context = 0;

  rt_return_value_t ret = rt_allocate_context(&context);
  assert(ret == RT_RET_NOERROR);

  ret = rt_add_callback(context, NN_FUNCTION_AFFINE, cb_alloc);
  assert(ret == RT_RET_NOERROR);

  nn_network_t *net = (nn_network_t *)affine_nnb;
  ret = rt_initialize_context(context, net);
  assert(ret == RT_RET_NOERROR);

  assert(rt_num_of_input(context) == 1);
  nn_variable_t *ivar = rt_input_variable(context, 0);
  assert(ivar->type == NN_DATA_TYPE_FLOAT);
  assert((rt_input_size(context, 0) * sizeof(float)) == sizeof(input));
  memcpy(rt_input_buffer(context, 0), input, sizeof(input));

  ret = rt_forward(context);
  assert(ret == RT_RET_NOERROR);

  ret = rt_free_context(&context);
  assert(ret == RT_RET_NOERROR);

  return 0;
}

/// @}
