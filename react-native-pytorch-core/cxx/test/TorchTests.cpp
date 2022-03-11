/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <fmt/format.h>
#include <gtest/gtest.h>
#include <string>

#include "TorchliveTestBase.h"

namespace {

class TorchliveRuntimeTest : public torchlive::test::TorchliveBindingsTestBase {
};

TEST_F(TorchliveRuntimeTest, TorchObjectTest) {
  EXPECT_TRUE(eval("typeof torch === 'object'").getBool());
}

TEST_F(TorchliveRuntimeTest, TensorShapeTest) {
  EXPECT_EQ(eval("torch.rand([5]).shape[0]").getNumber(), 5);
}

TEST_F(TorchliveRuntimeTest, TensorSizeTest) {
  EXPECT_TRUE(eval("torch.rand([5]).size()[0] === 5").getBool());
}

TEST_F(TorchliveRuntimeTest, TensorDtypeTest) {
  EXPECT_EQ(eval("torch.rand([5]).dtype").asString(*rt).utf8(*rt), "float32");
  EXPECT_EQ(
      eval("torch.rand([5], {dtype:'float64'}).dtype").asString(*rt).utf8(*rt),
      "float64");
}

TEST_F(TorchliveRuntimeTest, TorchEmptyTest) {
  // Valid inputs
  EXPECT_TRUE(eval("torch.empty([1,2]).shape[0] === 1").getBool());
  EXPECT_TRUE(eval("torch.empty([1,2]).shape[1] === 2").getBool());
  EXPECT_TRUE(eval("torch.empty(1,2).shape[0] === 1").getBool());
  EXPECT_TRUE(eval("torch.empty(1,2).shape[1] === 2").getBool());
  EXPECT_EQ(
      eval("torch.empty([1,2], {dtype:'float64'}).dtype")
          .asString(*rt)
          .utf8(*rt),
      "float64");
  EXPECT_EQ(
      eval("torch.empty(1,2, {dtype:'float64'}).dtype").asString(*rt).utf8(*rt),
      "float64");

  // Invalid inputs
  EXPECT_THROW(
      eval("torch.empty(1,2,true, {dtype:'float64'}).dtype"),
      facebook::jsi::JSError);
  EXPECT_THROW(
      eval("torch.empty(1,2,[1], {dtype:'float64'}).dtype"),
      facebook::jsi::JSError);
  EXPECT_THROW(
      eval("torch.empty({dtype:'float64'}, 1).dtype"), facebook::jsi::JSError);
  EXPECT_THROW(
      eval("torch.empty({dtype:'float64'}).dtype"), facebook::jsi::JSError);
}

TEST_F(TorchliveRuntimeTest, TensorDataTest) {
  EXPECT_TRUE(eval("torch.rand([5]).data instanceof Float32Array").getBool());
  EXPECT_TRUE(
      eval(
          "torch.rand([5], {dtype: torch.double}).data instanceof Float64Array")
          .getBool());
}

TEST_F(TorchliveRuntimeTest, TensorToStringTest) {
  EXPECT_TRUE(eval("typeof torch.rand([5]).toString() === 'string'").getBool());
}

TEST_F(TorchliveRuntimeTest, TorchArangeTest) {
  for (auto i = 0; i < 4; i++) {
    std::string withDtype =
        fmt::format("torch.arange(4, {{dtype:'float64'}}).data[{}]", i);
    std::string withoutDtype = fmt::format("torch.arange(4).data[{}]", i);
    EXPECT_EQ(eval(withDtype.c_str()).getNumber(), i);
    EXPECT_EQ(eval(withoutDtype.c_str()).getNumber(), i);
  }
  EXPECT_EQ(eval("torch.arange(4, {dtype:'float64'}).shape[0]").getNumber(), 4);
  EXPECT_EQ(
      eval("torch.arange(4, {dtype:'float64'}).dtype").asString(*rt).utf8(*rt),
      "float64");

  for (auto i = 0; i < 3; i++) {
    std::string withDtype =
        fmt::format("torch.arange(2, 5, {{dtype:'float64'}}).data[{}]", i);
    std::string withoutDtype = fmt::format("torch.arange(2, 5).data[{}]", i);
    int expectedVal = i + 2;
    EXPECT_EQ(eval(withDtype.c_str()).getNumber(), expectedVal);
    EXPECT_EQ(eval(withoutDtype.c_str()).getNumber(), expectedVal);
  }
  EXPECT_EQ(eval("torch.arange(2, 5).shape[0]").getNumber(), 3);
  EXPECT_EQ(
      eval("torch.arange(2, 5, {dtype:'float64'}).dtype")
          .asString(*rt)
          .utf8(*rt),
      "float64");

  for (auto i = 0; i < 5; i++) {
    std::string withDtype = fmt::format(
        "torch.arange(1, 3.5, 0.5, {{dtype:'float64'}}).data[{}]", i);
    std::string withoutDtype =
        fmt::format("torch.arange(1, 3.5, 0.5).data[{}]", i);
    double expectedVal = 1 + (i * 0.5);
    EXPECT_EQ(eval(withDtype.c_str()).getNumber(), expectedVal);
    EXPECT_EQ(eval(withoutDtype.c_str()).getNumber(), expectedVal);
  }
  EXPECT_EQ(eval("torch.arange(1, 3.5, 0.5).shape[0]").getNumber(), 5);
  EXPECT_EQ(
      eval("torch.arange(1, 3.5, 0.5, {dtype:'float64'}).dtype")
          .asString(*rt)
          .utf8(*rt),
      "float64");
}

TEST_F(TorchliveRuntimeTest, TorchRandintTest) {
  EXPECT_EQ(eval("torch.randint(10, [2, 2]).shape[0]").getNumber(), 2);
  EXPECT_EQ(eval("torch.randint(10, [2, 2]).shape[1]").getNumber(), 2);

  // Test randint with parameters high and size.
  for (auto i = 0; i < 4; i++) {
    std::string s = fmt::format("torch.randint(10, [2, 2]).data[{}]", i);
    std::string sWithDtype = fmt::format(
        "torch.randint(10, [2, 2], {{dtype:'float64'}}).data[{}]", i);
    EXPECT_GE(eval(s.c_str()).getNumber(), 0);
    EXPECT_LT(eval(s.c_str()).getNumber(), 10);
    EXPECT_GE(eval(sWithDtype.c_str()).getNumber(), 0);
    EXPECT_LT(eval(sWithDtype.c_str()).getNumber(), 10);
  }
  EXPECT_EQ(
      eval("torch.randint(10, [2, 2], {dtype:'float64'}).dtype")
          .asString(*rt)
          .utf8(*rt),
      "float64");

  // Test randint with parameters low, high, and size.
  EXPECT_EQ(eval("torch.randint(3, 5, [3]).shape[0]").getNumber(), 3);
  for (auto i = 0; i < 3; i++) {
    std::string s = fmt::format("torch.randint(3, 5, [3]).data[{}]", i);
    std::string sWithDtype = fmt::format(
        "torch.randint(3, 5, [3], {{dtype:'float64'}}).data[{}]", i);
    EXPECT_GE(eval(s.c_str()).getNumber(), 3);
    EXPECT_LT(eval(s.c_str()).getNumber(), 5);
    EXPECT_GE(eval(sWithDtype.c_str()).getNumber(), 3);
    EXPECT_LT(eval(sWithDtype.c_str()).getNumber(), 5);
  }
  EXPECT_EQ(
      eval("torch.randint(3, 5, [3], {dtype:'float64'}).dtype")
          .asString(*rt)
          .utf8(*rt),
      "float64");

  EXPECT_EQ(eval("torch.randint(3, 7, [3, 3, 4]).shape[2]").getNumber(), 4);
  for (auto i = 0; i < 36; i++) {
    std::string s = fmt::format("torch.randint(3, 7, [3, 3, 4]).data[{}]", i);
    EXPECT_GE(eval(s.c_str()).getNumber(), 3);
    EXPECT_LT(eval(s.c_str()).getNumber(), 7);
  }

  EXPECT_THROW(eval("torch.randint(1)"), facebook::jsi::JSError);
  EXPECT_THROW(eval("torch.randint(3, 7)"), facebook::jsi::JSError);
  EXPECT_THROW(eval("torch.randint([1], [3])"), facebook::jsi::JSError);
}

TEST_F(TorchliveRuntimeTest, TensorSqueezeTest) {
  EXPECT_TRUE(eval("torch.rand([1,2,1,3,1]).shape.length === 5").getBool());
  EXPECT_TRUE(
      eval("torch.rand([1,2,1,3,1]).squeeze().shape.length === 2").getBool());
  EXPECT_TRUE(
      eval("torch.rand([1,2,1,3,1]).squeeze().shape[0] === 2").getBool());
  EXPECT_TRUE(
      eval("torch.rand([1,2,1,3,1]).squeeze(2).shape.length === 4").getBool());
  // if the spcified dim is not size 1, return a copy of the tensor
  EXPECT_TRUE(
      eval("torch.rand([1,2,1,3,1]).squeeze(1).shape.length === 5").getBool());
  EXPECT_TRUE(
      eval("torch.rand([1,2,1,3,1]).squeeze().shape[1] === 3").getBool());
  EXPECT_THROW(
      eval("torch.rand([1,2,1,3,1]).squeeze(1,2)"), facebook::jsi::JSError);
}

TEST_F(TorchliveRuntimeTest, TensorUnsqueezeTest) {
  EXPECT_TRUE(eval("torch.rand([1,2,3]).shape.length === 3").getBool());
  EXPECT_TRUE(
      eval("torch.rand([1,2,3]).unsqueeze(1).shape.length === 4").getBool());
  EXPECT_TRUE(
      eval("torch.rand([1,2,3]).unsqueeze(1).shape[0] === 1").getBool());
  EXPECT_TRUE(
      eval("torch.rand([1,2,3]).unsqueeze(1).shape[1] === 1").getBool());
  EXPECT_TRUE(
      eval("torch.rand([1,2,3]).unsqueeze(1).shape[2] === 2").getBool());
  EXPECT_TRUE(
      eval("torch.rand([1,2,3]).unsqueeze(1).shape[3] === 3").getBool());
  // wrong number of arguments
  EXPECT_THROW(
      eval("torch.rand([1,2,3]).unsqueeze(1,2)"), facebook::jsi::JSError);
  // argument out of range
  EXPECT_THROW(
      eval("torch.rand([1,2,3]).unsqueeze(4)"), facebook::jsi::JSError);
}

TEST_F(TorchliveRuntimeTest, TorchAddTest) {
  std::string torchAddCodeWithNumber =
      R"(
          const tensor = torch.arange(2);
          const result = torch.add(tensor, 2);
          result.data[0] == tensor.data[0] + 2;
        )";
  EXPECT_TRUE(eval(torchAddCodeWithNumber.c_str()).getBool());

  std::string torchAddCodeWithTensor =
      R"(
          const tensor1 = torch.arange(2);
          const tensor2 = torch.arange(2);
          const result = torch.add(tensor1, tensor2);
          result.data[0] == tensor1.data[0] + tensor2.data[0];
        )";
  EXPECT_TRUE(eval(torchAddCodeWithTensor.c_str()).getBool());

  std::string torchAddCodeWithNumberAlpha =
      R"(
          const tensor1 = torch.arange(2);
          const result = torch.add(tensor1, 2, {alpha: 2});
          (result.data[0] == tensor1.data[0] + 2 * 2) && (result.data[1] == tensor1.data[1] + 2 * 2);
        )";
  EXPECT_TRUE(eval(torchAddCodeWithNumberAlpha.c_str()).getBool());

  std::string torchAddCodeWithTensorAlpha =
      R"(
          const tensor1 = torch.arange(2);
          const tensor2 = torch.arange(2);
          const result = torch.add(tensor1, tensor2, {alpha: 2});
          (result.data[0] == tensor1.data[0] + 2 * tensor2.data[0]) && (result.data[1] == tensor1.data[1] + 2 * tensor2.data[1]);
        )";
  EXPECT_TRUE(eval(torchAddCodeWithTensorAlpha.c_str()).getBool());

  EXPECT_THROW(eval("torch.add()"), facebook::jsi::JSError);
  EXPECT_THROW(eval("torch.add(1)"), facebook::jsi::JSError);
  EXPECT_THROW(
      eval("torch.add(torch.empty(1, 2), 'some_string')"),
      facebook::jsi::JSError);

  std::string torchAddCodeWithInvalidAlpha =
      R"(
          const tensor1 = torch.arange(2);
          const tensor2 = torch.arange(2);
          const result = torch.add(tensor1, tensor2, {alpha: 'random_string'});
        )";
  EXPECT_THROW(
      eval(torchAddCodeWithInvalidAlpha.c_str()), facebook::jsi::JSError);
}

TEST_F(TorchliveRuntimeTest, TorchSubTest) {
  std::string torchSubCodeWithNumber =
      R"(
          const tensor = torch.arange(2);
          const result = torch.sub(tensor, 2);
          result.data[0] == tensor.data[0] - 2;
        )";
  EXPECT_TRUE(eval(torchSubCodeWithNumber.c_str()).getBool());

  std::string torchSubCodeWithTensor =
      R"(
          const tensor1 = torch.arange(2);
          const tensor2 = torch.arange(2);
          const result = torch.sub(tensor1, tensor2);
          result.data[0] == tensor1.data[0] - tensor2.data[0];
        )";
  EXPECT_TRUE(eval(torchSubCodeWithTensor.c_str()).getBool());

  std::string torchSubCodeWithNumberAlpha =
      R"(
          const tensor1 = torch.arange(2);
          const result = torch.sub(tensor1, 2, {alpha: 2});
          (result.data[0] == tensor1.data[0] - 2 * 2) && (result.data[1] == tensor1.data[1] - 2 * 2);
        )";
  EXPECT_TRUE(eval(torchSubCodeWithNumberAlpha.c_str()).getBool());

  std::string torchSubCodeWithTensorAlpha =
      R"(
          const tensor1 = torch.arange(2);
          const tensor2 = torch.arange(2);
          const result = torch.sub(tensor1, tensor2, {alpha: 2});
          (result.data[0] == tensor1.data[0] - 2 * tensor2.data[0]) && (result.data[1] == tensor1.data[1] - 2 * tensor2.data[1]);
        )";
  EXPECT_TRUE(eval(torchSubCodeWithTensorAlpha.c_str()).getBool());

  EXPECT_THROW(eval("torch.sub()"), facebook::jsi::JSError);
  EXPECT_THROW(eval("torch.sub(1)"), facebook::jsi::JSError);
  EXPECT_THROW(
      eval("torch.sub(torch.empty(1, 2), 'some_string')"),
      facebook::jsi::JSError);

  std::string torchSubCodeWithInvalidAlpha =
      R"(
          const tensor1 = torch.arange(2);
          const tensor2 = torch.arange(2);
          const result = torch.sub(tensor1, tensor2, {alpha: 'random_string'});
        )";
  EXPECT_THROW(
      eval(torchSubCodeWithInvalidAlpha.c_str()), facebook::jsi::JSError);
}

TEST_F(TorchliveRuntimeTest, TorchMulTest) {
  std::string torchMulWithNumber =
      R"(
          const tensor = torch.arange(10);
          const result = torch.mul(tensor, 10);
          result.data[0] == tensor.data[0] * 10;
        )";
  EXPECT_TRUE(eval(torchMulWithNumber.c_str()).getBool());

  std::string torchMulWithTensor =
      R"(
          const tensor1 = torch.arange(2);
          const tensor2 = torch.arange(2);
          const result = torch.mul(tensor1, tensor2);
          result.data[0] == tensor1.data[0] * tensor2.data[0];
        )";
  EXPECT_TRUE(eval(torchMulWithTensor.c_str()).getBool());

  EXPECT_THROW(eval("torch.mul()"), facebook::jsi::JSError);
  EXPECT_THROW(eval("torch.mul(1)"), facebook::jsi::JSError);
  EXPECT_THROW(
      eval("torch.mul(torch.arrange(3, 4), 'foo')"), facebook::jsi::JSError);
}

TEST_F(TorchliveRuntimeTest, TorchSoftmaxTest) {
  std::string torchSoftmaxEachValueLessThanOne =
      R"(
          const tensor = torch.arange(2);
          const result = torch.softmax(tensor, 0);
          (result.data[0] <= 1 && result.data[0] >= 0) && (result.data[1] <= 1 && result.data[1] >= 0);
        )";
  EXPECT_TRUE(eval(torchSoftmaxEachValueLessThanOne.c_str()).getBool());

  std::string torchSoftmaxSumOfValuesEqualToOne =
      R"(
          const tensor = torch.arange(2);
          const result = torch.softmax(tensor, 0);
          Math.round(result.data[0] + result.data[1]);
        )";
  EXPECT_EQ(eval(torchSoftmaxSumOfValuesEqualToOne.c_str()).getNumber(), 1);

  EXPECT_THROW(eval("torch.softmax()"), facebook::jsi::JSError);
  EXPECT_THROW(eval("torch.softmax(1)"), facebook::jsi::JSError);
  EXPECT_THROW(
      eval("torch.softmax(torch.empty(1, 2), [1])"), facebook::jsi::JSError);
}

TEST_F(TorchliveRuntimeTest, TorchTensorTest) {
  std::string torchCreateTensorFromArrayShape =
      R"(
          const tensor = torch.tensor([[[1, 2, 3], [3, 4, 5]], [[1, 2, 3], [3, 4, 5]]]);
          tensor.shape[0] == 2 && tensor.shape[1] == 2 && tensor.shape[2] == 3;
        )";
  EXPECT_TRUE(eval(torchCreateTensorFromArrayShape.c_str()).getBool());
  std::string torchCreateTensorFromArrayData =
      R"(
          const tensor = torch.tensor([[[1, 2, 3], [3, 4, 5]], [[1, 2, 3], [3, 4, 5]]]);
          tensor.data[0] == 1 && tensor.data[1] == 2 && tensor.data[2] == 3 && tensor.data[3] == 3;
        )";
  EXPECT_TRUE(eval(torchCreateTensorFromArrayData.c_str()).getBool());
  std::string torchCreateTensorFromArrayDtype =
      R"(
          const tensor = torch.tensor([[[1, 2, 3], [3, 4, 5]], [[1, 2, 3], [3, 4, 5]]]);
          const tensor2 = torch.tensor([[[1.1, 2, 3], [3, 4, 5]], [[1, 2, 3], [3, 4, 5]]], {dtype: torch.int});
          const tensor3 = torch.tensor([[[1.1, 2, 3], [3, 4, 5]], [[1, 2, 3], [3, 4, 5]]], {dtype: torch.int64});
          tensor.dtype == torch.float32 && tensor2.dtype == torch.int && tensor2.data[0] == 1 && tensor3.dtype == torch.int64;
        )";
  EXPECT_TRUE(eval(torchCreateTensorFromArrayDtype.c_str()).getBool());
  EXPECT_THROW(
      eval(
          "const tensor = torch.tensor([[[1, 2, '3'], [3, 4, 5]], [[1, 2, 3], [3, 4, 5]]])"),
      facebook::jsi::JSError);
  EXPECT_THROW(
      eval("const tensor = torch.tensor([1, 2, '3'])"), facebook::jsi::JSError);
  EXPECT_THROW(
      eval("const tensor = torch.tensor([[1, 2, 3], 4])"),
      facebook::jsi::JSError);
  EXPECT_THROW(
      eval("const tensor = torch.tensor([[1, 2, 3], [1, 2]])"),
      facebook::jsi::JSError);
}

TEST_F(TorchliveRuntimeTest, TorchDivTest) {
  for (auto i = 0; i < 4; i++) {
    std::string torchDivWithNumber = fmt::format(
        R"(
          const tensor = torch.arange(1, 5);
          const result = torch.div(tensor, 2);
          result.data[{}] == tensor.data[{}] / 2;
        )",
        i,
        i);
    EXPECT_TRUE(eval(torchDivWithNumber.c_str()).getBool());
  }

  for (auto i = 0; i < 4; i++) {
    std::string torchDivWithNumberFloor = fmt::format(
        R"(
          const tensor = torch.arange(1, 5);
          const result = torch.div(tensor, 2, {{rounding_mode: 'floor'}});
          result.data[{}] == Math.floor(tensor.data[{}] / 2);
        )",
        i,
        i);
    EXPECT_TRUE(eval(torchDivWithNumberFloor.c_str()).getBool());
  }

  for (auto i = 0; i < 4; i++) {
    std::string torchDivWithTensor = fmt::format(
        R"(
          const tensor1 = torch.arange(1, 5);
          const tensor2 = torch.arange(1, 5);
          const result = torch.div(tensor1, tensor2);
          result.data[{}] == tensor1.data[{}] / tensor2.data[{}];
        )",
        i,
        i,
        i);
    EXPECT_TRUE(eval(torchDivWithTensor.c_str()).getBool());
  }

  for (auto i = 0; i < 4; i++) {
    std::string torchDivWithTensorTrunc = fmt::format(
        R"(
          const tensor1 = torch.arange(1, 5);
          const tensor2 = torch.arange(3, 7);
          const result = torch.div(
            tensor1,
            tensor2,
            {{rounding_mode: 'trunc'}});
          result.data[{}] == Math.trunc(tensor1.data[{}] / tensor2.data[{}]);
        )",
        i,
        i,
        i);
    EXPECT_TRUE(eval(torchDivWithTensorTrunc.c_str()).getBool());
  }

  std::string torchDivRoundingModeRandomVal = R"(
          const tensor1 = torch.arange(1, 5);
          const tensor2 = torch.arange(3, 7);
          const result = torch.div(
            tensor1,
            tensor2,
            {{rounding_mode: 'random_val'}});
        )";
  EXPECT_THROW(
      eval(torchDivRoundingModeRandomVal.c_str()), facebook::jsi::JSError);

  std::string torchDivInvalidTypeRoundingMode = R"(
          const tensor1 = torch.arange(1, 5);
          const tensor2 = torch.arange(3, 7);
          const result = torch.div(
            tensor1,
            tensor2,
            {{rounding_mode: 1}});
        )";
  EXPECT_THROW(
      eval(torchDivInvalidTypeRoundingMode.c_str()), facebook::jsi::JSError);

  EXPECT_THROW(eval("torch.div()"), facebook::jsi::JSError);
  EXPECT_THROW(eval("torch.div(1)"), facebook::jsi::JSError);
  EXPECT_THROW(
      eval("torch.div(torch.arange(3, 4), 'foo')"), facebook::jsi::JSError);
}

TEST_F(TorchliveRuntimeTest, TorchPermuteTest) {
  std::string torchPermute =
      R"(
          const tensor = torch.rand([2, 3, 1]);
          const result = torch.permute(tensor, [2, 0, 1]);
          const shape = result.shape;
          shape[0] === 1 && shape[1] === 2 && shape[2] === 3;
        )";
  EXPECT_TRUE(eval(torchPermute.c_str()).getBool());

  // Incorrect number of dims
  EXPECT_THROW(
      eval("torch.permute(torch.rand([2, 3, 1]), [0, 1])"),
      facebook::jsi::JSError);
  // Incorrect argument order
  EXPECT_THROW(
      eval("torch.permute([2, 0, 1], torch.rand([2, 3, 1]))"),
      facebook::jsi::JSError);
  // Incorrect call
  EXPECT_THROW(eval("torch.permute(1)"), facebook::jsi::JSError);
}

TEST_F(TorchliveRuntimeTest, TorchAbs) {
  std::string torchAbs =
      R"(
          let tensor = torch.tensor([[-2 ,-1], [0, 1]]);
          let output = torch.abs(tensor);
          output.data[0] == 2 && output.data[1] == 1 && output.data[2] == 0 && output.data[3] == 1
        )";
  EXPECT_TRUE(eval(torchAbs.c_str()).getBool());
  EXPECT_THROW(
      eval("torch.abs(torch.tensor([-1]), torch.tensor([1]));"),
      facebook::jsi::JSError);
  EXPECT_THROW(eval("torch.abs();"), facebook::jsi::JSError);
  EXPECT_THROW(eval("torch.abs(1);"), facebook::jsi::JSError);
  EXPECT_THROW(eval("torch.abs([1, 2, 3]);"), facebook::jsi::JSError);
}

TEST_F(TorchliveRuntimeTest, TensorAbs) {
  std::string torchAbs =
      R"(
          let tensor = torch.tensor([[-2, -1], [0, 1]]);
          let output = tensor.abs();
          output.data[0] == 2 && output.data[1] == 1 && output.data[2] == 0 && output.data[3] == 1
        )";
  EXPECT_TRUE(eval(torchAbs.c_str()).getBool());
  EXPECT_THROW(
      eval("torch.tensor([[-2 ,-1], [0, 1]]).abs(1);"), facebook::jsi::JSError);
}

TEST_F(TorchliveRuntimeTest, TorchTopkTest) {
  std::string torchTopkValid =
      R"(
          const tensor = torch.arange(10, 20);
          const [values, indices] = torch.topk(tensor, 3);
          (values.data[0] == 19 && values.data[1] == 18 && values.data[2] == 17) && (indices.data[0] == 9 && indices.data[1] == 8 && indices.data[2] == 7);
        )";
  EXPECT_TRUE(eval(torchTopkValid.c_str()).getBool());

  EXPECT_THROW(eval("torch.topk()"), facebook::jsi::JSError);
  EXPECT_THROW(eval("torch.topk(1)"), facebook::jsi::JSError);
  EXPECT_THROW(
      eval("torch.topk(torch.empty(1, 2), [1])"), facebook::jsi::JSError);
}
} // namespace
